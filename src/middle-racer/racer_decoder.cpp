#include "racer_decoder.h"

Racer_Decoder::Racer_Decoder(size_t control_group_ID_,
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
    vector <size_t> tmp0;
    this->active_regfiles.push_back(tmp0);
  }
};


/**
* @brief Create a sequence of micro-ops that will copy the bitmask stored in the
* shared torus buffer between the LSB and MSB tile into a column whose address is specified by the user.
*
* The sequence also propagate the mask from LSB to MSB so that all following instructions are predicated
*/
void Racer_Decoder::prepare_mask() {
  deque <string> tmp;
  if(this->ISA->command == "GETMASK") {
    string mask_addr = to_string(this->ISA->mask_addr);
    tmp.push_back("LOCK");
    tmp.push_back("BUF R r" + mask_addr);
    tmp.push_back("FILTER 0");
    tmp.push_back("BUF c0 R");
    tmp.push_back("UNFILTER");
    tmp.push_back("BUF r" + mask_addr + " L");
    tmp.push_back("RELEASE");
  }
  else if (this->ISA->command == "SETMASK") {
    string mask_addr = to_string(this->ISA->mask_addr);
    tmp.push_back("LOCK");
    tmp.push_back("NOP");
    tmp.push_back("FILTER 0");
    tmp.push_back("BUF r" + mask_addr + " R");
    tmp.push_back("UNFILTER");
    tmp.push_back("BUF R r" + mask_addr);
    tmp.push_back("BUF r" + mask_addr + " L");
    tmp.push_back("FILTER 0");
    tmp.push_back("BUF c0 R");
    tmp.push_back("UNFILTER");
    tmp.push_back("MASK r" + mask_addr);
    tmp.push_back("NOP");
    tmp.push_back("RELEASE");
  }
  // else if(this->ISA->command == "MASK") {
  //   string mask_addr = to_string(this->ISA->mask_addr);
  //   tmp.push("LOCK");
  //   string mask_load_command = "BUF R r" + mask_addr; // TODO: figure if we actually need mask columns
  //   tmp.push(mask_load_command);
  //   mask_load_command = "BUF r" + mask_addr + " L";
  //   tmp.push(mask_load_command);
  //   mask_load_command = "FILTER 0"; // reset the buffer clean
  //   tmp.push(mask_load_command);
  //   mask_load_command = "BUF c0 R";
  //   tmp.push(mask_load_command);
  //   mask_load_command = "UNFILTER";
  //   tmp.push(mask_load_command);
  //   tmp.push("MASK r" + mask_addr);
  //   tmp.push("RELEASE");
  // }
  else {
    tmp.push_back("UNMASK");
  }
  this->arith_template_queue = tmp;
  this->state = DECODER_START;
}

void Racer_Decoder::issue_to_backend(size_t lane_idx) {
	this->lanes[lane_idx]->receive(this->active_regfiles[lane_idx], this->uop_to_be_issued);
}

/**
* @brief Create a sequence of micro-ops that will make the LSB tile execute NOP
* for a number of cycle specified by the programmer
*/
void Racer_Decoder::prepare_nop() {
  deque <string> tmp;
  tmp.push_back("FILTER 0");
  for (size_t i = 0; i < this->ISA->delay_cycle; i++) {
    tmp.push_back("NOP");
  }
  tmp.push_back("UNFILTER");
  this->arith_template_queue = tmp;
  this->state = DECODER_START;
}

void Racer_Decoder::connect_lanes(vector <Cluster*> lanes_) {
  this->lanes = lanes_;
}

bool Racer_Decoder::check_stall() {
  bool stall = false;
  for (size_t i = 0; i < this->active_regfiles.size(); i+=1) {
    if (!this->active_regfiles[i].empty()) {
      stall |= this->lanes[i]->uop_queue->system_stall;
    }
  }
  return stall;
}

bool Racer_Decoder::is_connected_backend_active() {
  bool active = false;
  for (size_t i = 0; i < this->active_regfiles.size(); i+=1) {
    if (!this->active_regfiles[i].empty()) {
      active |= this->lanes[i]->uop_queue->is_active();
    }
  }
  return active;
}

string Racer_Decoder::show_uop_to_be_issued() {
  return this->uop_to_be_issued.raw;
}

/**
* @brief Replace the templated addresses with real address
*
* The function will also merge RELEASE and LOCK psuedo-micro-op with with the previous/next
* micro-op by specifying the packet_signal
*/
void Racer_Decoder::translate() {
  if (this->arith_template_queue.empty()) {
  this->state = DECODER_READY;
  }
  else {
    // Preprocessing: check for LOCK merging -----------------------------------

    this->state = DECODER_CONSTRUCT;
    string temp = this->arith_template_queue.front();
    this->arith_template_queue.pop_front();
    Racer_Uop uop;
    vector <string> temp_fields;
    // Split temp into fields
    size_t pos = 0;
    string token;
    string delimiter = " ";

    // If template uop is LOCK, don't actually send it to backend
    // set the packet_signal of the next instruction accordingly
    if (temp == "LOCK") {
      temp = this->arith_template_queue.front();
      this->arith_template_queue.pop_front();
      uop.packet_signal = "LOCK";
    }

    // Main: translate micro-op template to actual micro-ops -------------------
    while ((pos = temp.find(delimiter)) != std::string::npos) {
        token = temp.substr(0, pos);
        temp_fields.push_back(token);
        temp.erase(0, pos + delimiter.length());
    }
    temp_fields.push_back(temp);

    if (temp_fields[0] == "FILTER") {
      uop.type = PIPELINECTRL;
      uop.command = temp_fields[0];
      uop.field_length = 2;
      uop.tile_id = stoi(temp_fields[1]);
      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
    }
    else if (temp_fields[0] == "UNFILTER") {
      uop.type = PIPELINECTRL;
      uop.command = temp_fields[0];
      uop.field_length = 1;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
    }
    else if (temp_fields[0] == "MASK") {
      uop.type = PREDICATE;
      uop.command = temp_fields[0];
      uop.field_length = 2;
      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
    }
    else if (temp_fields[0] == "UNMASK") {
      uop.type = PREDICATE;
      uop.command = temp_fields[0];
      uop.field_length = 1;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
    }
    else if (temp_fields[0] == "NOP") {
      uop.type = UOP_NOP;
      uop.command = temp_fields[0];
      uop.field_length = 1;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
    }
    else if (temp_fields[0] == "BUF" || temp_fields[0] == "COPY") {
      uop.type = TRANSFER;
      uop.command = temp_fields[0];
      uop.field_length = 3;

      // replace this with more complex mapping if need be
      size_t colA_addr = this->ISA->arith_opA;
      size_t colB_addr = this->ISA->arith_opB;
      size_t colC_addr = this->ISA->arith_opC;

      // start replacement if needed
      if (temp_fields[1] == "A") {
        temp_fields[1] = "r" + to_string(colA_addr);
      }
      else if (temp_fields[1] == "B") {
        temp_fields[1] = "r" + to_string(colB_addr);
      }
      else if (temp_fields[1] == "C") {
        temp_fields[1] = "r" + to_string(colC_addr);
      }

      if (temp_fields[2] == "A") {
        temp_fields[2] = "r" + to_string(colA_addr);
      }
      else if (temp_fields[2] == "B") {
        temp_fields[2] = "r" + to_string(colB_addr);
      }
      else if (temp_fields[2] == "C") {
        temp_fields[2] = "r" + to_string(colC_addr);
      }

      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
      // uop.mov_src = temp_fields[1];
      // uop.mov_dest = temp_fields[2];
    }
    else {
      uop.type = BOOLEAN;
      uop.command = temp_fields[0];
      uop.field_length = 4;
      uop.fields = temp_fields;

      size_t colA_addr = this->ISA->arith_opA;
      size_t colB_addr = this->ISA->arith_opB;
      size_t colC_addr = this->ISA->arith_opC;

      // start replacement if needed
      for (size_t i = 0; i < temp_fields.size(); i+=1) {
        if (temp_fields[i] == "A") {
          // uop.bool_colA = colA_addr;
          temp_fields[i] = "r" + to_string(colA_addr);
        }
        else if  (temp_fields[i] == "B") {
          // uop.bool_colB = colB_addr;
          temp_fields[i] = "r" + to_string(colB_addr);
        }
        else if  (temp_fields[i] == "C") {
          // uop.bool_colC = colC_addr;
          temp_fields[i] = "r" + to_string(colC_addr);
        }
      }
      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
    }

    // Postprocessing: Check if STALL and RELEASE should be add to the micro-op

    // set stall signal so the next arithmetic instruction has to slow down
    if (this->arith_template_queue.empty()) {
      uop.stall_signal = "STALL";
    }
    else {
      // If this is the final uop in the packet, set the packet_signal to release
      if (this->arith_template_queue.front() == "RELEASE"){
         // pop the sudo micro-op RELEASE out for merge, and check if it is empty for stall again
        this->arith_template_queue.pop_front();
        uop.packet_signal = "RELEASE";
        if (this->arith_template_queue.empty()) {
          uop.stall_signal = "STALL";
        }
      }
    }

    // optimization: PREDICATE type micro-op don't really need stall since no WAR hazard possible
    // so set back to None
    if (uop.type == PREDICATE) {
      uop.stall_signal = "";
    }
    this->uop_to_be_issued = uop;
  }
}
bool Racer_Decoder::fetch_torus_evaluation(int lane, int regfile) {
  bool ret = true;
  size_t granularity = this->lanes[lane]->granularity;
  Column * T_reg = this->lanes[lane]->pipelines[regfile]->buffers[granularity - 1];
  for (int i = 0; i < T_reg->field.size(); i+=1) {
    ret &= (T_reg->field[i] == 1);
  }
  return ret;
}
