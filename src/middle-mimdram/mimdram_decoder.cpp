#include "mimdram_decoder.h"

Mimdram_Decoder::Mimdram_Decoder(size_t control_group_ID_,
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
  
  this->template_uprogram_pointer  = 0;
  this->elemsize_reg = (int) granularity_;
  this->bit_pos_reg = this->elemsize_reg ;
  this->nop_loop_reg = 0;

  if (this->map_style != AMBIT_ROWCLONE) {
  	printf("ERROR: Decoder: the selected logic family not valid for this micro-architecture\n");
  	exit(-1);
  }

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

// in MIMDRAM, the backend is always done finishing 
// the next cycle after the micro-op is issued
// so the mats are always not active when this function
// is called (there is no micro-op reuse/reissue like RACER)
bool Mimdram_Decoder::is_connected_backend_active() {
  return false;
}

// In MIMDRAM integration, we map the torus register
// to the DCC0 column, since dual-contact is useful for
// things like if-else statements, we don't need to perform 
// the "NOT" operation
void Mimdram_Decoder::prepare_mask() {
  deque <string> tmp;
  if(this->ISA->command == "GETMASK") {
    ;
  }
  else if (this->ISA->command == "SETMASK") {
    ;
  }
  else {
    tmp.push_back("UNMASK");
  }
  this->arith_template_queue = tmp;
  this->state = DECODER_START;
}

void Mimdram_Decoder::issue_to_backend(size_t lane_idx) {
  if (this->active_regfiles[lane_idx][0] != 0) {
    printf("ERROR: Decoder: MIMDRAM only has one register file per lane\n");
    exit(-1);
  }

  this->lanes[lane_idx]->execute(this->uop_to_be_issued);
}

// NOP is a simple control counter, 
// go to done if counter hits 0
void Mimdram_Decoder::prepare_nop() {
  ;
}

void Mimdram_Decoder::connect_lanes(vector <Mat*> lanes_) {
  this->lanes = lanes_;
}

// always false, MIMDRAM don't need to stall
// the frontend occasionally, unlike RACER
bool Mimdram_Decoder::check_stall() {
  return false;
}

// This function is physically mapped to the uProgram Processing Engine
// in MIMDRAM hardware
void Mimdram_Decoder::translate() {
  if (this->arith_template_queue.empty()) {
  this->state=DECODER_READY;
  }
  else {
    this->state = DECODER_CONSTRUCT;
    string temp = this->arith_template_queue[this->template_uprogram_pointer];
    Mimdram_Uop uop;
    vector <string> temp_fields;
    // Split temp into fields
    size_t pos = 0;
    string token;
    string delimiter = " ";

    // Main: translate micro-op template to actual micro-ops -------------------
    while ((pos = temp.find(delimiter)) != std::string::npos) {
        token = temp.substr(0, pos);
        temp_fields.push_back(token);
        temp.erase(0, pos + delimiter.length());
    }
    temp_fields.push_back(temp);

    if (temp_fields[0] == "NOP") {
      uop.type = MIMDRAM_UOP_CONTROL;
      uop.command = temp_fields[0];
      uop.field_length = 1;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
      this->template_uprogram_pointer += 1;
    }

    // Add use to increment the bit position of the input, output operands
    else if (temp_fields[0] == "SUBi") {
      int immediate = stoi(temp_fields[2]);

      // if (temp_fields[1] == "A") {
      //   this->ISA->arith_opA -= (size_t) immediate;
      // }
      // else if (temp_fields[1] == "B") {
      //   this->ISA->arith_opB -= (size_t) immediate;
      // }
      // else if (temp_fields[1] == "C") {
      //   this->ISA->arith_opC -= (size_t) immediate;
      // }
      if (temp_fields[1] == "BITPOS") {
        this->bit_pos_reg -= immediate;
      }
      else if (temp_fields[1] == "LOOPCOUNTER") {
      	this->nop_loop_reg -= immediate;
      	
      }
      else {
        printf("ERROR: Decoder: operand A must be the ISA operand address or the bit position register\n");
        exit(-1);
      }

      uop.type = MIMDRAM_UOP_ARITH;
      uop.command = temp_fields[0];
      uop.field_length = 3;
      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
      this->template_uprogram_pointer += 1;
    }

    // Control
    else if (temp_fields[0] == "BNZE") {
      if (temp_fields[1] == "BITPOS") {
	      if (this->bit_pos_reg != 0) {
		this->template_uprogram_pointer = stoi(temp_fields[2]);
	      }
	      else {
		this->template_uprogram_pointer += 1;
	      }   
      }
      else if (temp_fields[1] == "LOOPCOUNTER") {
      	if (this->nop_loop_reg != 0) {
		this->template_uprogram_pointer = stoi(temp_fields[2]);
	      }
	      else {
		this->template_uprogram_pointer += 1;
	      }   
      }
      else {
      	cout << "ERROR: Decoder: BNZE can only work with BITPOS and LOOPCOUNTER" << endl;
      	exit(-1);
      }

      uop.type = MIMDRAM_UOP_CONTROL;
      uop.command = temp_fields[0];
      uop.field_length = 3;
      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);
    }
    
    else if (temp_fields[0] == "SET_LOOPCOUNTER") {
      this->nop_loop_reg = stoi(temp_fields[1]);
      uop.type = MIMDRAM_UOP_CONTROL;
      uop.command = temp_fields[0];
      uop.field_length = 2;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
      this->template_uprogram_pointer += 1;
    }

    else if (temp_fields[0] == "DONE") {
      this->arith_template_queue.clear();
      this->bit_pos_reg = this->elemsize_reg;
      this->nop_loop_reg = 0;
      this->template_uprogram_pointer = 0;
      uop.type = MIMDRAM_UOP_CONTROL;
      uop.command = temp_fields[0];
      uop.field_length = 1;
      uop.fields = temp_fields;
      uop.raw = temp_fields[0];
    }

    // Boolean and Copy
    else if (temp_fields[0] == "RCLONE") {
      uop.type = MIMDRAM_UOP_RCLONE;
      uop.command = temp_fields[0];
      uop.field_length = 3;

      size_t colA_addr = this->ISA->arith_opA;
      size_t colB_addr = this->ISA->arith_opB;
      size_t colC_addr = this->ISA->arith_opC;

      // start replacement if needed
      if (temp_fields[1] == "A") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colA_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[1] == "B") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colB_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[1] == "C") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colC_addr + this->bit_pos_reg - 1);
      }


      if (temp_fields[2] == "A") {
        temp_fields[2] = "D" + to_string(this->elemsize_reg * colA_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[2] == "B") {
        temp_fields[2] = "D" + to_string(this->elemsize_reg * colB_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[2] == "C") {
        temp_fields[2] = "D" + to_string(this->elemsize_reg * colC_addr + this->bit_pos_reg - 1);
      }

      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);

      this->template_uprogram_pointer += 1;
    }
    
    else if (temp_fields[0] == "MAJOR") {
      uop.type = MIMDRAM_UOP_MAJOR;
      uop.command = temp_fields[0];
      uop.field_length = 2;

      size_t colA_addr = this->ISA->arith_opA;
      size_t colB_addr = this->ISA->arith_opB;
      size_t colC_addr = this->ISA->arith_opC;

      // start replacement if needed
      if (temp_fields[1] == "A") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colA_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[1] == "B") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colB_addr + this->bit_pos_reg - 1);
      }
      else if (temp_fields[1] == "C") {
        temp_fields[1] = "D" + to_string(this->elemsize_reg * colC_addr + this->bit_pos_reg - 1);
      }

      if (temp_fields.size() == 3) {
        uop.field_length = 3;
        if (temp_fields[2] == "A") {
          temp_fields[2] = "D" + to_string(this->elemsize_reg * colA_addr + this->bit_pos_reg - 1);
        }
        else if (temp_fields[2] == "B") {
          temp_fields[2] = "D" + to_string(this->elemsize_reg * colB_addr + this->bit_pos_reg - 1);
        }
        else if (temp_fields[2] == "C") {
          temp_fields[2] = "D" + to_string(this->elemsize_reg * colC_addr + this->bit_pos_reg - 1);
        }
      }

      uop.fields = temp_fields;
      uop.raw = vec2str(temp_fields);

      this->template_uprogram_pointer += 1;
    }
    this->uop_to_be_issued = uop;
  }
}

string Mimdram_Decoder::show_uop_to_be_issued() {
  return this->uop_to_be_issued.raw;
}

bool Mimdram_Decoder::fetch_torus_evaluation(int lane, int regfile) {
  if (regfile != 0) {
    printf("ERROR: Decoder: MIMDRAM only has one register file per lane\n");
    exit(-1);
  }

  bool ret = true;
  Column * T_reg = this->lanes[lane]->DCC0; // assign DCC0 as torus register
  for (int i = 0; i < T_reg->field.size(); i+=1) {
    ret &= (T_reg->field[i] == 1);
  }
  return ret;
}
