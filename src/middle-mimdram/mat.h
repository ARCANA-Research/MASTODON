/*  INTEGRATION NOTICE, PLEASE READ
For the sake of developer's sanity, the Mat is rotated 90 degree, such that bit-wise operations happen
between columns. This is to make sure MIMDRAM and RACER operates on the same compute direction, making
development much more manageable.
*/

#ifndef MAT_H
#define MAT_H

#include "pugixml.hpp"
#include <vector>
#include <sstream>
#include "mimdram_uop.h"
#include "../../util/config.h"

using namespace std;

class Mat
{
public:

  size_t granularity;

  size_t num_col;
  size_t num_row;

  size_t read_latency;
  size_t write_latency;
  size_t ID;

  Mat(string config_file_addr_, size_t ID_);

  // INTERGRATION NOTICE: these function signatures have to be as is, do not change
  // the function name or arguments type, list. If an argument is not needed
  // for your specific microarch, name it as __dummy_do_not_use__
  void tick();
  bool is_active();
  float report_activity_factor();
  //uint8_t get_byte(size_t bg, size_t row, size_t col);
  void toggle_backend(bool disable_signal);
  void toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  map <string, size_t> report_primitive_op_count();
  // data movement related
  Column * fetch_torus_register(int __dummy_do_not_use__);
  Column * fetch_regular_register(int __dummy_do_not_use__, int reg_idx, int bit_pos);

  // INTEGRATION NOTICE: these functions can be changes to adapt to your microarch
  void write_data(vector <string>  data);
  void display_data();
  void show_log(bool show_);
  void execute(Mimdram_Uop uOp);

  // D Group
  vector <Column *> d_group;
  // C Group
  vector <Column *> c_group;
  // B Group
  Column * T0;
  Column * T1;
  Column * T2;
  Column * T3;
  Column * DCC0;
  Column * DCC0_bar;
  Column * DCC1;
  Column * DCC1_bar;
  
private:
  vector <Column *> select_columns(string ureg);
  void write_to_node();
  void update_dual_contact();
  bool disable_signal;
  bool active;
  bool show;
  bool record;

  // Bookeeping
  pugi::xml_node this_node;
  pugi::xml_node parent_node;
  map <string, size_t> primitive_op_count;
};
#endif
