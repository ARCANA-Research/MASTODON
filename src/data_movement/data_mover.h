#ifndef DATA_MOVER_H
#define DATA_MOVER_H
#include "pugixml.hpp"
#include <vector>

#include "../top/pum_isa.h"
#include "../bottom/column.h"
#include "../middle-racer/cluster.h"
#include "../middle-mimdram/mat.h"

#include <type_traits>

using namespace std;

enum DataMoverType {SERIAL,
                    RING, // Don't use, implementation not complete
                    P2P,
                   };

template <class Back_End>
class Data_Mover
{
public:
  Data_Mover (size_t mover_COPY_lat_, size_t mover_SETUP_lat_){};
  void show_log(bool show_) {this->show = show_;};
  void record_log(bool record_) {this->record = record_;};
  void set_log_node(pugi::xml_node parent_node_) {
    this->parent_node = parent_node_;
    this->this_node = this->parent_node.append_child("Intra-Mover");
  };
  void write_to_node(string key, string value) {
    if (this->record == true) {
      this->this_node.append_child(key.c_str()).text() = value.c_str();
    }
  };

  virtual void get_instruction(PUM_ISA entry) = 0; // get instruction from controller's decoder
  virtual bool is_active() = 0; // let the controller know when things are done
  virtual void tick() = 0; // perform one cycle worth of work
  virtual void connect_lanes(vector <Back_End *>) = 0; // connect the mover to the underlying memory tiles
  virtual void get_src_des(size_t mov_src_lane, size_t mov_des_lane) = 0;
  virtual void erase_src_des() = 0;

  //bookeeping
  pugi::xml_node parent_node;
  pugi::xml_node this_node;
  bool show;
  bool record;
};
#endif
