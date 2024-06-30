#ifndef PUM_ISA_H
#define PUM_ISA_H
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;

/// Different type of RACER ISA
enum ISAtype {ARITHMETIC, ///< Arithmetic, pipelined operations
              MOVE, ///< Data movement operations
              COMPUTE, ///< Lane activation operations
              BRANCH, ///< Branch operations
              CFLOW, ///< Control flow operations (e.g., FLUSH)
              MPI, ///< Inter-Node interface
              NOP, ///< NOP operation,
              SUB ///< subroutine name
            };

class PUM_ISA
{
public:
  // General Info
  string raw;
  string command;
  vector <string> fields;
  size_t field_length;

  ISAtype type;

  // arithmetic ISA fields
  size_t arith_opA;
  size_t arith_opB;
  size_t arith_opC;

  // COMPUTE fields
  size_t compute_lane_id;
  size_t compute_regfile_id;

  // BRANCH fields
  size_t mask_addr;

  // NOP fields
  size_t delay_cycle;

  // JUMP
  size_t jump_addr;

  // data movement fields
  // MEM_COPY
  size_t mov_src_lane;
  size_t mov_src_regfile_id;
  size_t mov_src_reg_addr;
  size_t mov_des_lane;
  size_t mov_des_regfile_id;
  size_t mov_des_reg_addr;
  size_t mov_src_elem_idx;
  size_t shift_amount;

  // Node interface fields
  size_t network_id;
  PUM_ISA (string raw_);
  PUM_ISA ();
private:
  void check_field_length(string raw, size_t field_length, size_t expected_length);
};

#endif
