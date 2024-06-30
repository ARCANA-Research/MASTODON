#ifndef COLUMN_H
#define COLUMN_H
#include <vector>
#include <iostream>
#include <string>
#include <bitset>
#include <stdio.h>
#include <map>

#define MAX_COL_SIZE 128

using namespace std;

class Column
{
public:
  // General Info
  size_t col_size;
  string raw;
  vector <int> field;
  size_t binary;

  void write(size_t binary_); // writing in binary with different length is dangerous
  void write(string raw_);
  void write(vector <int> field_);
  void reset();
  Column(string raw, int col_size_);
  Column(vector <int> field, int col_size_);
  Column(size_t binary, int col_size_); // only use internally by operand overloading
  void copy(Column * src);
  void maj3(Column * src1, Column * src2);

  // Operator overloading
  // Boolean OR
  Column operator + (Column & obj);
  // Boolean AND
  Column operator * (Column & obj);
  // Boolean NOR
  Column operator | (Column & obj);
  // Boolean NAND
  Column operator & (Column & obj);
  // Boolean NOT
  Column operator ~ ();
  // Boolean XNOR
  Column operator ^ (Column & obj);

  Column operator = (Column result);

  int operator [] (size_t idx);
  friend ostream & operator << (ostream & os, Column const & col);

  // Member functions
  // void report();

  // Meta Data
  // map <string, size_t> write_op_count; // used to generate activity factor and column energy
  // map <string, size_t> read_op_count; // used to generate activity factor
  size_t ID;
};
#endif
