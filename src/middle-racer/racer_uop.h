#ifndef RACER_UOP_H
#define RACER_UOP_H
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;

/// RACER Micro-Op Types
enum Racer_Uop_Type {BOOLEAN, ///< In-memory primitives
              TRANSFER, ///< BUF: transfering data to and from tile/buffer
              PIPELINECTRL, ///< LOCK/RELEASE/STALL psuedo micro-op to be merged
              PREDICATE, ///< MASK instructions
              UOP_NOP ///< NOP instruction, nothing happen for one cycle
            };
class Racer_Uop
{
public:
  // General Info
  string raw;
  string command;
  string packet_signal;
  string stall_signal;
  vector <string> fields;
  size_t field_length;

  Racer_Uop_Type type;

  // PIPELINECTRL fields
  size_t tile_id;

  // Member function
  Racer_Uop(){};
  Racer_Uop(string raw_, string command_) { // for quick testing purposes
    this->raw = raw_;
    this->command = command_;
  }
private:
};

#endif
