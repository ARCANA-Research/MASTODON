#ifndef CLUSTER_H
#define CLUSTER_H
#include "bit_pipeline.h"
#include "uop_queue.h"
#include "../bottom/column.h"
#include "pugixml.hpp"
#include <vector>
#include <sstream>
#include "../../util/config.h"

using namespace std;

class Cluster
{
public:

  size_t granularity;
  size_t num_pipeline;
  size_t num_col;
  size_t num_row;
  size_t num_imm;
  size_t num_mask;
  size_t queue_size;
  size_t read_latency;
  size_t write_latency;
  size_t ID;
  vector < Bit_Pipeline * > pipelines;
  Uop_Queue * uop_queue;

  Cluster(string config_file_addr_, size_t ID_);
  void tick();
  // void switch_regfile(size_t pipeline_idx);
  void write_data(size_t pipeline_idx, vector <vector <string> > data);
  void display_data(size_t pipeline_idx);
  void show_log(bool show_);
  void toggle_backend(bool disable_signal);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  void receive(vector<size_t> active_regfiles, Racer_Uop uOp);

  void toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL);
  
  bool is_active();
  float report_activity_factor();

  // mainly used for testing, probably should be modified further
  uint8_t get_byte(size_t pipeline_idx, size_t bg, size_t row, size_t col);

  map <string, size_t> report_primitive_op_count();

  // Data movement related
  Column * fetch_torus_register(int pipeline_idx);
  Column * fetch_regular_register(int pipeline_idx, int reg_idx, int bit_pos);


private:
  pugi::xml_node this_node;
  pugi::xml_node parent_node;
  map <string, size_t> primitive_op_count;
};
#endif
