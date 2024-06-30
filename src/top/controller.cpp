#ifndef CONTROLLER_CPP
#define CONTROLLER_CPP
#include "controller.h"
/**
* @brief Constructor for the Controller class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Node class. The class has
* two main functions: (1) fetch an entry from the binary storage; (2) decode and issue
* micro-ops to the backend
*
* @param control_group_ID_ a unique ID (number) associated with the controller
* @param num_lane_ the number of processing lanes that the controller can send micro-ops to
* @param granularity the operating precision of the node
* @param the number of queue entry in the micro-op queue
* @param map_style_ different types of ISA-to-micro-op recipe style (e.g., MAGIC vs. OSCAR)
*/
template <class Back_End, class Decoder_Type>
Controller<Back_End, Decoder_Type>::Controller( size_t control_group_ID_,
                        size_t num_lane_,
                        size_t granularity_,
                        size_t playback_buffer_size_, 
                        size_t max_num_regfile_per_lane_,
                        size_t num_thread_,
                        PUMtech map_style_)
{
  this->control_group_ID = control_group_ID_;
  this->PC = 0;
  this->stamp_name = "";
  this->return_PC = 0;
  this->num_lane = num_lane_;
  this->show = false;
  this->record = false;
  this->outer_most_loop_addr = 0;
  this->num_thread = num_thread_;

  this->decoder =  new Decoder_Type(0, num_lane_, playback_buffer_size_, max_num_regfile_per_lane_, map_style_, granularity_);
  this->active_thread_ID = 0;

  this->fetcher = new Fetcher();
};

/**
* @brief Connect a controller to a Binary_Storage object for fetching
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::connect_binary_storage(Binary_Storage* binary_storage)
{
  this->connected_bin = binary_storage;
  this->fetcher->connect_binary_storage(binary_storage);
};

/**
* @brief Connect a controller to different processing lanes
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::connect_lanes(vector <Back_End *> lanes_) {
  this->lanes = lanes_;
  this->decoder->connect_lanes(lanes_);
};

/**
* @brief Connect a controller to the data moving engine
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::connect_mover(Data_Mover<Back_End> * mover_) {
  this->mover = mover_;
  this->decoder->mover = mover_;
};

/**
* @brief Connect a controller to the mpi server
*/
#if SST_ENABLED
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::connect_mpi_service(MPI_Server * mpi_server_) {
  this->mpi_server = mpi_server_;
  this->decoder->mpi_server = mpi_server_;
};
#endif
/**
* @brief Set the show flag used to printout to terminal
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::show_log(bool show_) {
  this->show = show_;
}

/**
* @brief Set the record flag used to record in an XML file
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::record_log(bool record_) {
  this->record = record_;
}

/**
* @brief Create xml node to use for record and to send to all child nodes
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::set_log_node(pugi::xml_node parent_node_) {
  this->parent_node = parent_node_;
  this->this_node = this->parent_node.append_child(("Controller_" + to_string(this->control_group_ID)).c_str());
}

/**
* @brief Write message to xml node
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::write_to_node(string key, string value) {
  if (this->record == true) {
    this->this_node.append_child(key.c_str()).text() = value.c_str();
  }
}

/**
* @brief Check if any of the processing lanes are being stalled
*
* If one or more lanes are stalled, then stall the entire node. This function is called
* every cycle, and assume that there is no stall to begin with, then check each lane for
* system_stall signal correction
*/
template <class Back_End, class Decoder_Type>
bool Controller<Back_End, Decoder_Type>::check_stall() {
  return this->decoder->check_stall();
}

template <class Back_End, class Decoder_Type>
bool Controller<Back_End, Decoder_Type>::is_backend_active() {
  for (size_t i = 0; i < this->num_lane; i+=1) {
    if (this->lanes[i]->is_active()) {
        return true;
    }
  }
  return false;
}

template <class Back_End, class Decoder_Type>
float Controller<Back_End, Decoder_Type>::report_activity_factor() {
  float total = 0.0;
  for (size_t i = 0; i < this->num_lane; i+=1) {
    total += this->lanes[i]->report_activity_factor();
  }
  return total / (float) this->num_lane;
}

/**
* @brief Advance the state of the controller by one cycle
*
* In each cycle, the controller does the following sequentially: (1) check for
* stall; (2) if the system is not stalled, then advance the fetcher and decdoer;
(3) manage the fetcher states; (4) manage the decoder states.
* The fetcher management is as follow: if the fetch is idle, then fetch a new instruction.
* Otherwise, if the fetcher has an instruction ready to be decode, then check the decoder.
* If the decoder is busy decoding something else, just wait. If the decoder is idle, then
* send the fetched entry to be decoded.
* The decoder management is as follow: If the decoder just output a new micro-op, issue it to the
* correct lanes. The decoder also tells the controller which lanes should be active.
*/
template <class Back_End, class Decoder_Type>
void Controller<Back_End, Decoder_Type>::tick()
{

  this->mover->tick(); // system stall due to harzard does not affect mover, since MOVE and MOVE_DONE protects movement
  this->decoder->tick();
  
  // System will stall to prevent a new instruction from crashing (using the micro-op queues) into a previous instruction
  // Stall only happen because of WAR hazzard, and is usually small, so no context switch needed
  if (!this->check_stall()) {

    this->fetcher->tick();
    // ===================================== Manage Fetcher's states =====================================
    // If fetcher is ready to fetch new instruction (i.e., it has sucessfully fetched an instruction)
    if (this->fetcher->state == FETCH_READY) {
      this->fetcher->fetch_attempt(this->PC);

      // Bookeeping, not important:
      this->write_to_node("Fetch", to_string(this->PC).c_str());
      if (this->show) {cout << "Controller: fetching : " << to_string(this->PC) << endl;}
    }

    // Else if fetcher is waiting to issue current instruction to decoder and this is not the last instruction (i.e., EOF)
    else if ((   this->fetcher->state == FETCH_FETCHED 
              || this->fetcher->state == FETCH_WAIT_DECODER)
              && (((string)(this->fetcher->fetched_entry.data)).find("EOF") == std::string::npos)) 
    {
      // Can the fetcher send the instruction to decoder?
      if (this->decoder->state == DECODER_READY) {
        this->decoder->get_instruction((string)(this->fetcher->fetched_entry.data), this->PC); // send instruction to decoder
        this->fetcher->state = FETCH_READY; // sucessfully sent, ready to fetch a new instruction

        // controller need to self-decode for jump instructions
        // we cannot use this->decoder->ISA because the jump_addr field will be changed for playback buffer
        PUM_ISA * ISA = new PUM_ISA((string)(this->fetcher->fetched_entry.data));
        this->PC++;

        // Bookeeping, not important:
        if (this->show) {cout << "Controller: decoding : " << (string) this->fetcher->fetched_entry.data << endl;}
        string fetched_string = this->fetcher->fetched_entry.data;
        this->write_to_node("Decode", fetched_string.erase(fetched_string.length() - 1));
      }

      // Else, wait for the decoder to be done
      else {
        this->fetcher->state = FETCH_WAIT_DECODER;
      }
    }
    // ===================================== Manage Decoder's states =====================================
    if (this->decoder->state == DECODER_CONSTRUCT) {
      // Bookeeping, not important:
      if (this->show) {cout << "Controller: issuing uop: " << this->decoder->show_uop_to_be_issued() << " to backend"<< endl;}
      this->write_to_node("Issue", this->decoder->show_uop_to_be_issued());
     
    }

    if (this->decoder->state == DECODER_FLUSH) {
      // only resume all activities when the micro-op queue is empty for RACER microarchitecture
      if (!this->is_backend_active()) {
        this->decoder->state = DECODER_READY;
      }

      // Bookeeping, not important:
      if (this->show) {cout << "Controller: Flushing!" << endl;}
      this->write_to_node("Idle", "Flush");
    }

    if (this->decoder->state == DECODER_LFLUSH) {
      // Bookeeping, not important:
      if (this->show) {cout << "Controller: Flushing!" << endl;}
      this->write_to_node("Idle", "Flush");
    }

    if (this->decoder->state == DECODER_WAIT4MOVE) {
      if (!this->mover->is_active()) {
        this->mover->erase_src_des();
        this->decoder->state = DECODER_READY;
      }

      // Bookeeping, not important:
      if (this->show) {cout << "Controller: Waiting for Mover!" << endl;}
      this->write_to_node("Idle", "Intra-Node");
    }

#if SST_ENABLED
    if (this->decoder->state == DECODER_WAIT4MPI) {
      // Bookeeping, not important:
      this->write_to_node("Idle", "Inter-Node");
    }
#endif
  }
  else {
    // Bookeeping, not important:
    if (this->show) {cout << "Controller: Stalling!" << endl;}
    this->write_to_node("Idle", "Stall");
  }
}
#endif
