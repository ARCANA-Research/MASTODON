#ifndef TILE_H
#define TILE_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <Python.h>
#include "pugixml.hpp"
#include "../bottom/column.h"
#include "racer_uop.h"
using namespace std;

class Tile
{
public:
  // General Info
  size_t num_col;
  size_t num_imm;
  size_t num_mask;
  size_t num_row;
  size_t tile_id;
  string cycle_time;
  string volt_MAGIC;
  string volt_ISO_BL;
  size_t analog_error_counter;

  Tile(size_t tile_id, size_t num_mask_, size_t num_imm_, size_t num_col_, size_t num_row_, Column * left_buffer_, Column * right_buffer_);

  Column * select_column(string column_name);
  void execute(Racer_Uop uop);
  // void report();
  map <string, size_t> primitive_op_count;
  map <string, size_t> report_primitive_op_count();
  void reset();

  // device-level flow functions
  std::pair<int, int> report_analog_error();
  void toggle_device_model_sim(bool device_model_sim_, bool memorisation_, string cycle_time, string volt_MAGIC, string volt_ISO_BL);
  void update_states(int tile_id);
  void update_states_binary(int tile_id);
  void simulate_verilogA(wchar_t* argv[]);
  void simulate_verilogA_memorisation(std::vector<std::string>& stimuli);
  wchar_t* string2widechar(const string& narrowString);
  void generate_verilogA_LOGIC(Racer_Uop uop, wchar_t ** stimuli);
  void generate_verilogA_LOGIC_memorisation(Racer_Uop uop, std::vector<std::string>& stimuli);
  void generate_verilogA_WRITE(Racer_Uop uop, wchar_t ** stimuli);
  void generate_verilogA_WRITE_memorisation(Racer_Uop uop, std::vector<std::string>& stimuli);
  void update_tile_state();

  void write_data(vector <string> data);
  void activate_predication(string mask_id);
  void deactivate_predication();
  void display_data();
  bool is_MSB();
  void set_MSB(bool MSBtile_);
  uint8_t get_bit(size_t row, size_t col);
// private:
  bool predicated;
  bool MSB_tile;
  bool device_model_sim;
  bool memorisation;
  vector <Column> columns;
  vector <Column> immediates;
  vector <Column> masks;
  vector <Column> constants;
  Column * active_mask;
  Column * left_buffer;
  Column * right_buffer;
};
#endif
