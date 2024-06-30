#include "pum_isa.h"

void PUM_ISA::check_field_length(string raw, size_t field_length, size_t expected_length) {
  if (field_length - 1 != expected_length) { // command field doenst count
    printf("ERROR: Racer ISA: unexpected field length\n");
    cout << raw << endl;
    exit(-1);
  }
}

PUM_ISA::PUM_ISA() {};

/**
* @brief Constructor for the PUM_ISA class
*
* This constructor parse through a string entry and populate the ISA object accordingly
*/
PUM_ISA::PUM_ISA(string raw_) {
  // if (raw_.find(":") != std::string::npos) {
  //       // printf("RACER ISA: illegal instruction\n");
  //       // cout << raw_ << endl;
  //       // exit(-1);
  //   return;
  // } 
  // Split instruction into fields
  this->raw = raw_;
  size_t pos = 0;
  string token;
  string delimiter = " ";
  while ((pos = raw_.find(delimiter)) != std::string::npos) {
      token = raw_.substr(0, pos);
      this->fields.push_back(token);
      raw_.erase(0, pos + delimiter.length());
  }
  this->fields.push_back(raw_);
  this->field_length = this->fields.size();
  this->command = this->fields[0];

  // Arithmetic configuration ISA
  if (this->command.find("COMPUTE") != std::string::npos) {
    this->type = COMPUTE;
    if (this->command == "COMPUTE") {
      this->check_field_length(this->raw, this->field_length,2);
      sscanf(this->fields[1].c_str(), "%zu", &(this->compute_lane_id));
      sscanf(this->fields[2].c_str(), "%zu", &(this->compute_regfile_id));
    }
    else if (this->command == "COMPUTE_ALL") {
      this->check_field_length(this->raw, this->field_length,1);
      sscanf(this->fields[1].c_str(), "%zu", &(this->compute_regfile_id));
    }
    else {
      this->check_field_length(this->raw, this->field_length,0);
      this->command.erase(this->command.length() - 1);
    }
  }

  // Data movement configuration ISA
  //else if (this->command.find("MEM_FENCE") != std::string::npos) {
   // this->type = MOVE;
  //}
  else if (this->command.find("MOVE") != std::string::npos){
    this->type = MOVE;
    if (this->command == "MOVE") {
      this->check_field_length(this->raw, this->field_length,2);
      sscanf(this->fields[1].c_str(), "%zu", &(this->mov_src_lane));
      sscanf(this->fields[2].c_str(), "%zu", &(this->mov_des_lane));
    }
    else {
      this->check_field_length(this->raw, this->field_length,0);
      this->command.erase(this->command.length() - 1);
    }
  }

  // Inter-Node interface
  else if (this->command.find("SEND") != std::string::npos){
    this->type = MPI;
    if (this->command == "MPI_SEND") {
      this->check_field_length(this->raw, this->field_length,3);
      sscanf(this->fields[1].c_str(), "%zu", &(this->network_id));
      sscanf(this->fields[2].c_str(), "%zu", &(this->mov_src_lane));
      sscanf(this->fields[3].c_str(), "%zu", &(this->mov_des_lane));
    }
    else {
      this->check_field_length(this->raw, this->field_length,0);
      this->command.erase(this->command.length() - 1);
    }
  }
  else if (this->command.find("MPI_RECV") != std::string::npos) {
    this->check_field_length(this->raw, this->field_length,1);
    this->type = MPI;
    sscanf(this->fields[1].c_str(), "%zu", &(this->network_id));
  }
  
  else if (this->command.find("MPI_BARRIER") != std::string::npos) {
    this->type = MPI;
    this->check_field_length(this->raw, this->field_length,0);
    this->command.erase(this->command.length() - 1);
  }
  
  else if (this->command.find("MPI_INTRP_REQ") != std::string::npos) {
  	this->type = MPI;
  	if (this->command == "MPI_INTRP_REQ") {
  		this->check_field_length(this->raw, this->field_length, 1);
  		sscanf(this->fields[1].c_str(), "%zu", &(this->network_id));
  	}
  	else {
	      this->check_field_length(this->raw, this->field_length,0);
	      this->command.erase(this->command.length() - 1);
    	}
  }
  else if (this->command.find("MPI_INTRP_SERVE") != std::string::npos) {
    this->type = MPI;
    this->check_field_length(this->raw, this->field_length,0);
    this->command.erase(this->command.length() - 1);
  }

  // Data movement ISA
  else if (this->command.find("MEM_COPY_RSHIFT") != std::string::npos) {
    this->check_field_length(this->raw, this->field_length,5);
    this->type = MOVE;
    sscanf(this->fields[1].c_str(), "%zu", &(this->mov_src_regfile_id));
    sscanf(this->fields[2].c_str(), "%zu", &(this->mov_src_reg_addr));
    sscanf(this->fields[3].c_str(), "%zu", &(this->shift_amount));
    sscanf(this->fields[4].c_str(), "%zu", &(this->mov_des_regfile_id));
    sscanf(this->fields[5].c_str(), "%zu", &(this->mov_des_reg_addr));
  }
  else if (this->command.find("MEM_COPY") != std::string::npos) {
    this->check_field_length(this->raw, this->field_length,4);
    this->type = MOVE;
    sscanf(this->fields[1].c_str(), "%zu", &(this->mov_src_regfile_id));
    sscanf(this->fields[2].c_str(), "%zu", &(this->mov_src_reg_addr));
    sscanf(this->fields[3].c_str(), "%zu", &(this->mov_des_regfile_id));
    sscanf(this->fields[4].c_str(), "%zu", &(this->mov_des_reg_addr));
  }
  else if (this->command.find("MEM_BCAST") != std::string::npos) {
    this->check_field_length(this->raw, this->field_length,5);
    this->type = MOVE;
    sscanf(this->fields[1].c_str(), "%zu", &(this->mov_src_regfile_id));
    sscanf(this->fields[2].c_str(), "%zu", &(this->mov_src_reg_addr));
    sscanf(this->fields[3].c_str(), "%zu", &(this->mov_src_elem_idx));
    sscanf(this->fields[4].c_str(), "%zu", &(this->mov_des_regfile_id));
    sscanf(this->fields[5].c_str(), "%zu", &(this->mov_des_reg_addr));
  }

  // Control flow ISA
  else if ((this->command.find("FLUSH") != std::string::npos) ||
            (this->command.find("RETURN") != std::string::npos)) {
    this->check_field_length(this->raw, this->field_length,0);
    this->command.erase(this->command.length() - 1);
    this->type = CFLOW;
  }

  else if (this->command.find("JUMP") != std::string::npos) {
    this->check_field_length(this->raw, this->field_length,1);
    sscanf(this->fields[1].c_str(), "%zu", &(this->jump_addr));
    this->type = CFLOW;
  }
  else if (this->command == "NOP") {
    this->check_field_length(this->raw, this->field_length,1);
    this->type = NOP;
    sscanf(this->fields[1].c_str(), "%zu", &(this->delay_cycle));
  }


  // Branch-related ISA
  else if (this->command.find("MASK") != std::string::npos){
    this->type = BRANCH;
    if (this->command == "GETMASK" || this->command == "SETMASK") {
      this->check_field_length(this->raw, this->field_length,1);
      sscanf(this->fields[1].c_str(), "%zu", &(this->mask_addr));
    }
    if (this->command.find("UNMASK") != std::string::npos){
      this->check_field_length(this->raw, this->field_length,0);
      this->command.erase(this->command.length() - 1);
    }
  }

  else if (this->command.find(":") != std::string::npos){
    this->type = SUB;
  }

  // Multi-processing ISA
  //else if ((this->command.find("FORK") != std::string::npos) ||
  //         (this->command.find("EXIT") != std::string::npos)){
  //  this->type = MULTIPROCESS;
  //}

  // Arithmetic ISA
  else {
    this->type = ARITHMETIC;
    if (this->fields.size() == 1) {
      this->check_field_length(this->raw, this->field_length,0);
      this->command.erase(this->command.length() - 1);
    }
    if (this->fields.size() > 1) {
      sscanf(this->fields[1].c_str(), "%zu", &(this->arith_opA));
    }
    if (this->fields.size() > 2) {
      sscanf(this->fields[2].c_str(), "%zu", &(this->arith_opB));
    }
    if (this->fields.size() > 3) {
      sscanf(this->fields[3].c_str(), "%zu", &(this->arith_opC));
    }
    else {
      this->arith_opC = -1;
    }
  }
}
