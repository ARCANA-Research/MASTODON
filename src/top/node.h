#ifndef Node_H
#define Node_H

#include "controller.h"
#include "binary_storage.h"
//INTEGRATION NOTICE: Add your lane class and decoder class here
#include "../middle-racer/cluster.h"
#include "../middle-mimdram/mat.h"
#include "../middle-racer/racer_decoder.h"
#include "../middle-mimdram/mimdram_decoder.h"

#include "../data_movement/data_mover.h"
#include "../data_movement/serial_mover.h"
#include "../data_movement/ring_mover.h"
#include "../data_movement/p2p_mover.h"
#include "../../util/init.h"
#include "../../util/config.h"

// #include "../util/pretty.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <type_traits>
#include "pugixml.hpp"
#include "stdarg.h"

#define BIN_ENTRY_SIZE 64 // unit is character

using namespace std;

template <class Back_End, class Decoder_Type>
class Node
{
public:
  size_t control_group_ID;
  vector <Back_End *> lanes; // lanes are always accessible for data movement and initialization
  size_t idle_counter;

  Binary_Storage * bin;
  Controller<Back_End, Decoder_Type> * controller;
  Data_Mover<Back_End> * mover;
/*
  // Default Constructor
  Node(size_t control_group_ID_);

  // Complex Constructor
  Node(size_t control_group_ID_,
                size_t num_lane_,
                PUMtech map_style_,
                size_t total_entries_,
                size_t output_latency_,
                size_t input_latency_,
                size_t granularity_,
                size_t queue_size_,
                size_t num_regfile_,
                size_t num_col_,
                size_t num_row_,
                size_t num_imm_,
                size_t num_mask_,
                size_t random_bit_pos_,
                size_t mover_COPY_lat_,
                size_t mover_SETUP_lat_,
                size_t playback_buffer_size,
                size_t max_num_regfile_per_lane,
                size_t num_thread_,
                DataMoverType mover_type_);
*/
  // Runtime config Constructor
  Node(size_t control_group_ID_, string config_file_addr);

  // void init_lane(string data_file_address, size_t lane_ID);
  int load_to_bin(string binary_file);
  // void set_max_idle_time(size_t num_idle_cycle);
  void set_record(bool flag, string fields);
  void set_show(bool flag, string fields);
  void set_log_node(pugi::xml_node node);
  void tick();

  // Report functions
  float report_activity_factor(size_t num_clock_cycle);
  map <string, size_t> report_primitive_op_count();

//private:

  // Controller config
  size_t num_lane;
  PUMtech map_style;
  bool disable_signal;
  bool device_model_sim;
  bool memorisation;
  
  string config_file_addr;

  // Binary storage config
  size_t total_entries;
  size_t output_latency;
  size_t input_latency;
  bool loading_bin;
  FILE * opened_bin_file;
  char entry_to_store [BIN_ENTRY_SIZE + 1];
  const size_t data_size = sizeof(char) * BIN_ENTRY_SIZE;

  // Lane config
  size_t granularity;
  size_t num_regfile;
  size_t num_col;
  size_t num_row;
  size_t num_imm;
  size_t num_mask;
  string cycle_time;
  string volt_MAGIC;
  string volt_ISO_BL;

  // Playback config
  size_t playback_buffer_size;
  size_t max_num_regfile_per_lane;

  // Hyper-threading config
  size_t num_thread;

  // Mover config
  DataMoverType mover_type;
  size_t mover_COPY_lat;
  size_t mover_SETUP_lat;

  // Performance
  float activity_factor;
  map <string, size_t> primitive_op_count;

  void setup(size_t random_bit_pos);
};
#endif
#include "node.cpp"
