#ifndef BIT_PIPELINE_H
#define BIT_PIPELINE_H
#include "tile.h"
#include "../bottom/column.h"
#include "racer_uop.h"
#include "pugixml.hpp"
#include <string>
#include <vector>
#include <sstream>

using namespace std;
class Bit_Pipeline {
public:
  Bit_Pipeline(size_t granularity_, size_t num_row_, size_t num_col_, size_t num_mask_, size_t num_imm_, size_t ID_);
  void write_data(vector <vector <string> > data);
  void display_data();
  void execute(size_t tile_idx, Racer_Uop uop);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  void write_to_node();
  void toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL);
  uint8_t get_byte(size_t row, size_t col, size_t bg);
  map <string, size_t> report_primitive_op_count();
  size_t num_row;
  size_t num_col;
  size_t num_imm;
  size_t num_mask;
  size_t granularity;
  size_t ID;
  // Column * out_buffer;
// private:
  vector <Tile *> tiles;
  vector <Column *> buffers;
  bool record;
  pugi::xml_node this_node;
  pugi::xml_node parent_node;
  map <string, size_t> primitive_op_count;
  // Column * in_buffer;
};

#endif
