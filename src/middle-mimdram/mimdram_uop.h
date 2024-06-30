#ifndef MIMDRAM_UOP_H
#define MIMDRAM_UOP_H
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;

/// RACER Micro-Op Types
enum Mimdram_Uop_Type {MIMDRAM_UOP_RCLONE,
                       MIMDRAM_UOP_MAJOR,
                       MIMDRAM_UOP_ARITH,
                       MIMDRAM_UOP_CONTROL
                     };
class Mimdram_Uop
{
public:
  // General Info
  string raw;
  string command;
  
  vector <string> fields;
  size_t field_length;

  Mimdram_Uop_Type type;

  // Member function
  Mimdram_Uop(){};
};

#endif
