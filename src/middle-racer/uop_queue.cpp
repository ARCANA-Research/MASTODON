#include "uop_queue.h"

using namespace std;

/**
* @brief Constructor for Uop_Queue class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Cluster class. The class has a single
* role: propagting instructions through the micro-op queues in the correct manner.
* Check the Uop_Queue::lazy_copy_when_issue() function for more details.
*
* @param pipeline_length_ the number of micro-op queues needed
* @param queue_size_ the number of micro-op entries inside each queue
*/
Uop_Queue::Uop_Queue(size_t pipeline_length_,
                       size_t queue_size_)
                       // size_t read_latency_,
                       // size_t write_latency_)
{
  this->pipeline_length = pipeline_length_;
  this->queue_size = queue_size_;
  this->record = false;
  this->system_stall = false;
  this->active = false;
  this->backend_disable = false;
  this->activity_factor = 0.0;
  this->stalling_queue = this->pipeline_length; // no queue from 0 -> length - 1 is staling

  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    deque <Racer_Uop> uOp_queue;
    this->uOp_queues.push_back(uOp_queue);
    this->lock_signals.push_back(true);
    this->filter_signals.push_back(false);
    this->filter_targets.push_back(false);
    this->stall_signals.push_back(false);

    // this->lock_counters.push_back(0);
    this->release_counters.push_back(0);
    this->staging.push_back(uOp_queue);
  }

  // LSB queue is never lock
  this->lock_signals[0] = false;

  this->show = false;
};

/**
* @brief Set the record flag used to record in an XML file
*/
void Uop_Queue::record_log(bool record_){
  this->record = record_;
}

/**
* @brief Connect clusters to the shared micro-op queue
*/
void Uop_Queue::connect_pipelines(vector <Bit_Pipeline *> pipelines) {
  this->pipelines = pipelines;
}

/**
* @brief activate the correct lanes, regfiles to listen to uop
* @param active_lanes a set of active local lane id
* @param active_regfiles the set of regfiles that the lanes are pointing to
*/
void Uop_Queue::receive(vector<size_t> active_regfiles, Racer_Uop uop) {
  this->receive_uOp(uop);
  this->active_pipelines.clear();
  for (unsigned int i = 0; i < active_regfiles.size(); i+=1) {
    this->active_pipelines.push_back(this->pipelines[active_regfiles[i]]);
  }
}

/**
* @brief Create xml node to use for record and to send to all child nodes
*/
void Uop_Queue::set_log_node(pugi::xml_node parent_node_) {
  this->parent_node = parent_node_;
};

/**
* @brief Write message to xml node
*/
void Uop_Queue::write_to_node() {
  if (this->record == true) {
    this->this_node = this->parent_node.append_child("Uop_Queues");
    pugi::xml_node activity_node = this->this_node.append_child("Activity");
    for (size_t i = 0; i < this->pipeline_length; i+=1) {
      activity_node.append_child(("Queue_" + to_string(i)).c_str()).text() = to_string(this->uOp_queues[i].size()).c_str();
    }

    if (this->uOp_queues[0].size() > 0) {
      Racer_Uop uop = this->uOp_queues[0].front();
      if (uop.type == BOOLEAN) {
        this->this_node.append_child("Boolean").text() = "1";
        this->this_node.append_child("Buffer").text() = "0";
        this->this_node.append_child("Filter").text() = "0";
        this->this_node.append_child("Predicate").text() = "0";
        this->this_node.append_child("NOP").text() = "0";
      }
      else if (uop.type == TRANSFER) {
        this->this_node.append_child("Boolean").text() = "0";
        this->this_node.append_child("Buffer").text() = "1";
        this->this_node.append_child("Filter").text() = "0";
        this->this_node.append_child("Predicate").text() = "0";
        this->this_node.append_child("NOP").text() = "0";
      }
      else if (uop.type == PIPELINECTRL) {
        this->this_node.append_child("Boolean").text() = "0";
        this->this_node.append_child("Buffer").text() = "0";
        this->this_node.append_child("Filter").text() = "1";
        this->this_node.append_child("Predicate").text() = "0";
        this->this_node.append_child("NOP").text() = "0";
      }
      else if (uop.type == PREDICATE) {
        this->this_node.append_child("Boolean").text() = "0";
        this->this_node.append_child("Buffer").text() = "0";
        this->this_node.append_child("Filter").text() = "0";
        this->this_node.append_child("Predicate").text() = "1";
        this->this_node.append_child("NOP").text() = "0";
      }
      else if (uop.type == UOP_NOP) {
        this->this_node.append_child("Boolean").text() = "0";
        this->this_node.append_child("Buffer").text() = "0";
        this->this_node.append_child("Filter").text() = "0";
        this->this_node.append_child("Predicate").text() = "0";
        this->this_node.append_child("NOP").text() = "1";
      }
    }
    else {
      this->this_node.append_child("Boolean").text() = "0";
      this->this_node.append_child("Buffer").text() = "0";
      this->this_node.append_child("Filter").text() = "0";
      this->this_node.append_child("Predicate").text() = "0";
      this->this_node.append_child("NOP").text() = "0";
    }
  }
}

/**
* @brief Get micro-op from Controller
*
* This function is called by the Controller
*
* @return -1 when the first micro-op queue is full. This will make the conroller/Node stall
* @return 0 means the new micro-op has been sucessfully recieved
*/
int Uop_Queue::receive_uOp(Racer_Uop uOp)
{
  // Check if we can push one more uOp in to the queue chain
  if (this->uOp_queues[0].size() == this->queue_size) {
    printf("WARNING: LSB queue is full\n");
    return -1;
  }

  this->uOp_queues[0].push_back(uOp);
  return 0;
};

float Uop_Queue::report_activity_factor() {
  return this->activity_factor;
}

void Uop_Queue::toggle_backend(bool disable_signal) {
  this->backend_disable = disable_signal;
}

/**
* @brief micro-op queue chains behavior that is updated every cycle
*
* This function is the core of the mico-op queue chain. It works on two important
* member objects: staging and uOp_queues. The staging vector holds the micro-op to be inserted
* to the next queue. You can think of the staging as the registers in between combinational blocks in Verilog.
* The staging object is neccessary for us to implement a blocking assignment every cycle. Basically, the staging
* prevents a value from being propagated from the LSB queue to the MSB queue directly in one cycle.
*
* There are three major Uop_Queue behaviors that this function implement:
* (1) If the head micro-op is LOCK, and there is no RELEASE inside the queue,
* then the queue will not do anything (no issuing to tile, or commit the micro-op to staging);
* (2) If the head micro-op is FILTER, and the queue is the target of the FILTER,
* then the queue issue that and all subsequent micro-op without commiting to stagging. This
* prevent the micro-ops from propgating further into the queue chains. This behavior will stop
* when there is an UNFILTER at the head of the queue;
* (3) If there is a stalling innstruction in this queue and the previous queue is going
* to commit to staging, then all previous queues + the controller has to be stalled.
*
* These three behavior works indpendently of each other, meaning they are all enforce all at the same time.
* The behaviors are sequentially checked from the LSB to the MSB queues.
*
* Look at the actual implementation in the code base for more comments on the function
*/
void Uop_Queue::lazy_copy_when_issue() {
  this->activity_factor = 0.0;
    //Sequential for loop to get the next uops
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    this->activity_factor += (float) this->uOp_queues[i].size();
    // queue 0 does not having staging for micro-op
    // staging is needed to make the microop queue chain sequential (sequetial definition as in Verilog)
    if (i != 0) {
      if (!this->staging[i].empty()) {
        if (this->stall_signals[i] == false) {
          this->uOp_queues[i].push_back(this->staging[i].front());
        }
        this->staging[i].pop_front();
      }
    }
  }

  this->activity_factor /= ((float) this->pipeline_length * (float) this->queue_size);

  // For every cycle, all queues do this (in parallel, since the previous sequential loop take care of dependency)
  // First, assume that there is no stalling hazzard
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    this->stall_signals[i] = false;
    this->system_stall = false;
  }
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    // first and foremost, if I am part of the stalled group, don't do anything
    if (this->stall_signals[i] == true) {
      continue;
    }

    // if I am empty, do nothing
    if (this->uOp_queues[i].empty()) {
      continue;
    }

    // Check to see if I should be locked or not
    if (i != 0) { // skip queue 0, it is never lock
      if ((this->release_counters[i] == 0) && (this->uOp_queues[i].front().packet_signal == "LOCK")) {
        this->lock_signals[i] = true;
      }
      else {
        this->lock_signals[i] = false;
      }
    }

    // If I am not locked and there is something to be issued
    if (this->lock_signals[i] == false && this->uOp_queues[i].size() > 0) {
      Racer_Uop issued_uop = this->uOp_queues[i].front();

      // FILTER/UNFILTER routine
      if (issued_uop.command == "UNFILTER") {
        this->filter_signals[i] = false;
      }
      // if I am not the targeted queue, don't do anything
      else if (issued_uop.command == "FILTER" && issued_uop.tile_id != i) {
        this->filter_signals[i] = true;
      }
      // if I am the target, then execute the following uops
      else if (issued_uop.command == "FILTER" && issued_uop.tile_id == i) {
        this->filter_signals[i] = false;
        this->filter_targets[i] = true;
      }

      // copy to next queue
      // decrement counters before erasing
      if (i != 0) {
        if (issued_uop.packet_signal == "LOCK") {
          this->release_counters[i] -= 1;
        }
      }

      if (i < this->pipeline_length - 1) {
        // optimization: stop pushing filtered ops through the queue if has already been executed
        // if I am not the target, then always push back
        if (this->filter_targets[i] == false) {
          if (this->staging[i+1].size() != 0) {
            printf("Uop_queue: ERROR: staging is busy\n");
            exit(-1);
          }

          // before stage commit, check if there is a stalling hazard
          if (this->uOp_queues[i+1].size() != 0) {
            if (this->uOp_queues[i+1].back().stall_signal == "STALL") {
              for (size_t j = 0; j <= i; j+= 1) {
                this->stall_signals[j] = true;
              }
              this->system_stall = true;
              continue;
            }
          }

          // commit to staging
          this->staging[i+1].push_back(issued_uop);
        }
        // if I am the target, dont push anything to next queue until the UNFILTER is reached
        else if ((this->filter_targets[i] == true) && (issued_uop.command == "UNFILTER")) {
          this->filter_targets[i] = false;

          // if the UNFILTER has a stall signal, permit it to move on, since stall is word-wide
          if (issued_uop.stall_signal == "STALL") {
            if (this->staging[i+1].size() != 0) {
              printf("Uop_queue: ERROR: staging is busy\n");
              exit(-1);
            }
            this->staging[i+1].push_back(issued_uop);
          }
        }

        if (issued_uop.packet_signal == "RELEASE") {
          this->release_counters[i+1] += 1;
        }
      }

      // only execute if it is not a filtered uop
      if (this->filter_signals[i] == false) {
        if(!(this->backend_disable)) { // optionally disable backend for simulation speedup
          for (unsigned int p = 0; p < this->active_pipelines.size(); p+=1) {
            this->active_pipelines[p]->execute(i, issued_uop);
          }
        }
      }
    }
  }
};

/**
* @brief Pop all of the head micro-ops if the queues are not locked
*/
void Uop_Queue::pop_fronts() {
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    if (this->lock_signals[i] == false
        && this->uOp_queues[i].size() > 0
        && this->stall_signals[i] == false) {
      this->uOp_queues[i].pop_front();
    }
  }
}

/**
* @brief Function called by Node::tick() to see if the miro-op queue chain is active
* @return true if there is something inside the queue chain
* @return false if there is nothing inside the queue chain. The Node may increment
* its idle counter so that the whole simulation retire after a certain number of idle cycles
*/
bool Uop_Queue::is_active() {
  return this->active;
};

/**
* @brief Set the show flag used to printout to terminal
*/
void Uop_Queue::show_log(bool show_) {
  this->show = show_;
};

/**
* @brief Check if there are micro-op somewhere inside the queue chain
*/
void Uop_Queue::check_active() {
  this->active = false;
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    if (this->uOp_queues[i].size() > 0) {
      this->active = true;
    }
    if (this->staging[i].size() > 0) {
      this->active = true;
    }
  }
}

/**
* @brief Check if one of the queue in the chain cannot recieve more micro-op
* @return -1 means a queue is full, a stall may be issued by the Controller
* @return 0 means no queue are full
*/
int Uop_Queue::check_queue_full() {
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    if (this->uOp_queues[i].size() == this->queue_size) {
      printf("WARNING: queue is full \n");
      return -1;
    }
  }
  return 0;
};

/**
* @brief Check if the entire chain of queues are empty
* @return -1 if there is one queue active
* @return 0 if queue chain is empty
*/
int Uop_Queue::check_queue_empty() {
  for (size_t i = 0; i < this->pipeline_length; i+=1) {
    if ((int) this->uOp_queues[i].size() != 0) {
      return -1;
    }
  }
  return 0;
};

/**
* @brief Display the queue chain's content for debug
*
* If the queue is red, it is locked. If the queue is green, it is issuing and committing normally.
* If the queue has a magenta micro-op, that micro-op is a stalling micro-op.
* The staging object is put on top of each queue.
*/
void Uop_Queue::print_uOp_queues() {
  if (this->show == true) {
    // 18 white spaces
    string blank_space("               ");
    string divider(" | ");
    cout << "\033[90m";
    cout << "    X";
    for (int i = this->pipeline_length - 1; i >= 0 ; i-=1) {
      if (this->staging[i].size() != 0) {
        string raw_uop = this->staging[i].front().raw;
        int uop_width = raw_uop.size();
        int extra_space = 15 - uop_width;
        cout << raw_uop;
        for (int k = 0 ; k < extra_space; k+=1) {
          cout << " ";
        }
        cout << " X ";
      }
      else {
        cout << blank_space << " X ";
      }
    }
    cout << "\033[0m"<< endl;

    for (int i = this->queue_size - 1; i >= 0; i-=1) {
      string slice = to_string(i);
      if (i < 10) {
        slice += " ";
      }
      slice += ": |";
      for (int j = this->pipeline_length - 1; j >= 0 ; j-=1) {
        if (i >= (int) this->uOp_queues[j].size()) {
          slice.append(blank_space);
          slice.append(divider);
          continue;
        }
        string raw_uop = this->uOp_queues[j][i].raw;
        if (this->uOp_queues[j][i].stall_signal == "STALL") {
          slice.append("\033[45m");
        }
        else {
          slice.append("\033[0m");
        }
        int uop_width = raw_uop.size();
        int extra_space = 15 - uop_width;
        if (this->lock_signals[j] == true) {
          slice.append("\033[31m");
        }
        else {
          slice.append("\033[32m");
        }
        slice.append(raw_uop);
        for (int k = 0 ; k < extra_space; k+=1) {
          slice.append(" ");
        }
        slice.append("\033[0m");
        slice.append(divider);
      }
      cout << slice << endl;
    }
    cout << "\033[0m";
  }
};

// void Uop_Queue::switch_pipeline(Bit_Pipeline* pipeline_) {
//   this->pipeline = pipeline_;
// }

/**
* @brief Advance the state of the Uop_Queue by one cycle
*/
void Uop_Queue::tick()
{
  this->check_active();
  // Check that every queue can recieve uOp
  if (check_queue_full() != 0) {
    this->system_stall = true;
  }

  if (this->active) {
    this->lazy_copy_when_issue();

    // bookeeping
    this->print_uOp_queues();
    this->write_to_node();

    this->pop_fronts();
    //this->check_active();
  }
};
