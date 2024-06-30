#ifndef DECODER_CPP
#define DECODER_CPP
#include "decoder.h"
/**
* @brief Constructor for Decoder class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Controller class. The class has three
* main functions: (1) decode an ISA into a stream of micro-op templates; (2) replace the templated
* addresses with real addresses; (3) issue one micro-op per cycle to the back end
*
* @param control_group_ID_ a unique ID (number) associated with the decoder
* @param num_lane_ the number of lanes that the Controller is connected to
* @param map_style_ different types of ISA-to-micro-op recipe style (e.g., MAGIC vs. OSCAR)
*
*/
template <class Back_End>
Decoder<Back_End>::Decoder(size_t control_group_ID_,
                 size_t num_lane_,
                 size_t playback_buffer_size_,
                 size_t max_num_regfile_per_lane_,
                 PUMtech map_style_,
                 size_t granularity_) {

  this->control_group_ID = control_group_ID_;
  this->num_lane = num_lane_;
  this->map_style = map_style_;
  this->state = DECODER_READY;
  this->playback_buffer_size = playback_buffer_size_;
  this->max_num_regfile_per_lane = max_num_regfile_per_lane_;

  this->playback = false;
  this->early_playback = false;
  this->playback_PC = 0;
  this->global_PC_offset = 0;

  this->recipe_table = new RecipeTable(map_style_);
  this->show = false;
  this->record = false;
  this->buffer_disabled = false;

  for (size_t i = 0; i < this->num_lane; i+=1) {
    vector <size_t> tmp;
    this->active_regfiles.push_back(tmp);
  }
};

template <class Back_End>
void Decoder<Back_End>::disable_buffer(bool flag) {
  this->buffer_disabled = flag;
}

/**
* @brief Recieve an instruction from the Controller
*
* @param entry the instruction the controller wants to send to the decoder
*
* @return -1 the decoder is busy decoding something else
* @return 0 the instruction is sucessfully recieved by the decoder, decoder enters DECODER_PREP stage
*/
template <class Back_End>
int Decoder<Back_End>::get_instruction(string entry, size_t PC) {
  if (this->state != DECODER_READY) {
    if(this->show){printf("Decoder busy, cannot get instruction");}
    return -1;
  }
  this->ISA = new PUM_ISA(entry);
  this->state = DECODER_PREP;
  return 0;
};

/**
* @brief Select the correct control flow based on the ISA type
*
*/
template <class Back_End>
void Decoder<Back_End>::prepare_ctrl_flow() {

  // if this is a  interupt request command, skip prepartion, just send the command to the des Node
#if SST_ENABLED
  if (this->mpi_server->interupt_request == true) {
  	if (this->ISA->command == "JUMP" || this->ISA->command == "JUMP_COND") {
  		printf("Error: Decoder: interupt request cannot be JUMP or JUMP_COND \n");
  		exit(-1);
  	}
  	this->prepare_mpi();
  }
#endif
  if (this->ISA->type == ARITHMETIC) {
    this->prepare_arithmetic();
  }

  else if (this->ISA->type == COMPUTE) {
    this->prepare_compute_config();
  }
  else if (this->ISA->type == BRANCH) {
    this->prepare_mask();
  }
  else if (this->ISA->type == NOP) {
    this->prepare_nop();
  }
  else if (this->ISA->type == CFLOW) {
    this->prepare_cflow();
  }
  else if (this->ISA->type == MOVE) {
    // MPI may use MEM_COPY of type MOVE instruction, so need to check
#if SST_ENABLED
    if (this->mpi_server->is_active()) {
      this->prepare_mpi();
    }
    else {
      this->prepare_mover();
    }
#else
   this->prepare_mover();
#endif
  }
#if SST_ENABLED
  else if (this->ISA->type == MPI) {
    this->prepare_mpi();
  }
#endif

  else if (this->ISA->type == SUB) {
    this->state = DECODER_READY;
  }
  // if instruction not supported
  else{
    printf("ERROR: ISA type not supported\n");
    cout << this->ISA->raw << endl;
    exit(-1);
  }
};

/**
* @brief Set the show flag used to printout to terminal
*/
template <class Back_End>
void Decoder<Back_End>::show_log(bool show_) {
  this->show = show_;
}

/**
* @brief Set the record flag used to record in an XML file
*/
template <class Back_End>
void Decoder<Back_End>::record_log(bool record_) {
  this->record = record_;
}

/**
* @brief Send instruction to data mover
*/
template <class Back_End>
void Decoder<Back_End>::prepare_mover() {
  if (this->ISA->command == "MOVE") {
    this->mover->get_src_des(this->ISA->mov_src_lane, this->ISA->mov_des_lane);
    this->state = DECODER_READY;
  }
  else if (this->ISA->command == "MOVE_DONE") {
    this->state = DECODER_WAIT4MOVE;
  }
  else {
    this->mover->get_instruction(*(this->ISA));
    this->state = DECODER_READY;
  }
}

/**
* @brief Send instruction to SST::Link
*/
#if SST_ENABLED
template <class Back_End>
void Decoder<Back_End>::prepare_mpi() {
  if (this->ISA->command == "MPI_SEND") {
    //cout << "SEND signal received" << endl;
    this->mpi_server->start_as_MPI_SEND(this->ISA->network_id, this->ISA->mov_src_lane, this->ISA->mov_des_lane);
    this->state = DECODER_READY;
  }
  else if (this->ISA->command == "MPI_SEND_DONE") {
    //cout << "SEND_DONE received" << endl;
    this->state = DECODER_WAIT4MPI;
    this->mpi_server->last_payload_request_received = true;
  }
  else if (this->ISA->command == "MPI_RECV") {
    //cout << "RECV signal received" << endl;
    this->mpi_server->start_as_MPI_RECV(this->ISA->network_id);
    this->state = DECODER_WAIT4MPI;
  }
  else if (this->ISA->command == "MPI_BARRIER") {
    this->mpi_server->start_as_MPI_BARRIER();
    this->state = DECODER_WAIT4MPI;
  }
  else if (this->ISA->command == "MPI_INTRP_REQ") {
  	this->mpi_server->start_as_MPI_INTRP_REQ(this->ISA->network_id);
  	this->state = DECODER_READY;
  }
  else if (this->ISA->command == "MPI_INTRP_REQ_DONE") {
  	this->state = DECODER_WAIT4MPI;
  	this->mpi_server->last_payload_request_received = true;
  }
  else if (this->ISA->command == "MPI_INTRP_SERVE") {
  	this->state = DECODER_WAIT4MPI;
  	this->mpi_server->start_as_MPI_INTRP_SERVE();
  }
  else {
    this->mpi_server->generate_payload(*(this->ISA));
    this->state = DECODER_READY;
  }
}
#endif

/**
* @brief Activate the correct lane or a group of lanes
*/
template <class Back_End>
void Decoder<Back_End>::prepare_compute_config() {
  if (this->ISA->command == "COMPUTE") {
    int lane_addr = (int) this->ISA->compute_lane_id;
    size_t regfile_addr = this->ISA->compute_regfile_id;


    if (this->active_regfiles[lane_addr].size() == (int) this->max_num_regfile_per_lane) {
        printf("ERROR: Decoder: too many active reg files \n");
        exit(-1);
    }
    // if there is space, put it in the active queue
    else {
      this->active_regfiles[lane_addr].push_back(regfile_addr);
    }

    this->state = DECODER_READY;
  }


  else if (this->ISA->command == "COMPUTE_ALL") {
    size_t regfile_addr = this->ISA->compute_regfile_id;
    for (size_t i = 0; i < this->num_lane; i+=1) {

      if (this->active_regfiles[i].size() == this->max_num_regfile_per_lane) {
          printf("ERROR: Decoder: too many active reg files \n");
          exit(-1);
      }
      // if there is space, put it in the active queue
      else {
        this->active_regfiles[i].push_back(regfile_addr);
      }
    }

    this->state = DECODER_READY;
  }

  else if (this->ISA->command == "COMPUTE_DONE") {
    for (size_t i = 0; i < this->num_lane; i+=1) {
      this->active_regfiles[i].clear();
    } 
    
    this->state = DECODER_READY;
  }
};

/**
* @brief Recieve the templated micro-op recipe of the ISA from Recipe_Table
*/
template <class Back_End>
void Decoder<Back_End>::prepare_arithmetic() {
  // empty mask mean the instruciton is for some other control group
  // if (this->active_lanes.empty()) {
  //   this->state = DECODER_READY;
  // }
  // // else, start preping the micro-op
  // else {
    // Get uOp template
    this->arith_template_queue = this->recipe_table->get_recipe(this->ISA->command);
    this->state = DECODER_START;
  // }
};

/**
* @brief Notify the Controller that there is a control flow instruction  that
* change the behavior of the Node, so that the controller can handle it
*/

template <class Back_End>
void Decoder<Back_End>::prepare_cflow() {
  if (this->ISA->command == "FLUSH") {
    this->state = DECODER_FLUSH;
  }
  else if (this->ISA->command == "LFLUSH") {
    this->state = DECODER_LFLUSH;
  }
  else {
  this->state = DECODER_READY;
   
  }
}

template <class Back_End>
string Decoder<Back_End>::vec2str(vector<string> input) {
  string ret = "";
  for (size_t i = 0; i < input.size(); i+=1) {
    ret += input[i];
    ret += " ";
  }
  return ret;
}

/**
* @brief Advance the state of the decoder by updating the decoder's FSM
*/
template <class Back_End>
void Decoder<Back_End>::tick() {

  if (!this->check_stall()) {
    switch (this->state) {
      case DECODER_READY: {
        if(this->show){cout << "Decoder: Ready" << endl;}
        this->state = DECODER_READY; // Only the controller can break this loop
        break;
      }

      case DECODER_PREP: {
        if(this->show){cout << "Decoder: Preparing" << this->ISA->raw << endl;}
        this->prepare_ctrl_flow();
        break;
      }

      case DECODER_START: { //START AND CONSTRUCT IS THE SAME
        if(this->show){cout << "Decoder: Constructing" << endl;}
        this->translate();
        break;
      }

      case DECODER_CONSTRUCT: {
        if(this->show){cout << "Decoder: Constructing" << endl;}
        this->translate();
        break;
      }

      case DECODER_FLUSH: {
        if(this->show){cout << "Decoder: Flushing" << endl;}
        this->state = DECODER_FLUSH; // Only the controller can break this loop
        break;
      }

    
      case DECODER_LFLUSH: {
        if(this->show){cout << "Decoder: Local Flushing" << endl;}
        if (!this->is_connected_backend_active()) {
            this->state = DECODER_READY;

        }
        else {
          this->state = DECODER_LFLUSH;
        }
        break;
      }


      case DECODER_WAIT4MOVE: {
        if(this->show){cout << "Decoder: Waiting for data mover" << endl;}
        this->state = DECODER_WAIT4MOVE; // Only the controller can break this loop
        break;
      }

      case DECODER_WAIT4MPI: {
        if(this->show){cout << "Decoder: Waiting for MPI server" << endl;}
        this->state = DECODER_WAIT4MPI; // Only the Node wrapper can break this loop
        break;
      }
    }

    // Issue micro-op to all activated lanes in the compute region
    if (this->state == DECODER_CONSTRUCT) {
      //#pragma omp parallel for
      for (size_t i = 0; i < this->active_regfiles.size(); i+=1) {
          if (!this->active_regfiles[i].empty()) {
            this->issue_to_backend(i);
          }
      }
    }
  }
}
#endif
