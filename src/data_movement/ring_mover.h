/*
TODO: Need to finish implementing this at somepoint
Right now it is broken, do not use RING type movement
*/

#ifndef RING_MOVER_H
#define RING_MOVER_H

#include <queue>
#include <vector>
#include "./data_mover.h"
#include <utility>

#define NUM_NODE 8

using namespace std;

template <class Back_End>
class Ring_Mover: public Data_Mover<Back_End>
{
public:
  Ring_Mover(size_t mover_COPY_lat_, size_t mover_SETUP_lat_);
  void get_instruction(PUM_ISA entry) override;
  bool is_active() override;
  void tick() override;
  void connect_lanes(vector<Back_End *> lanes_) override;
  void get_src_des(size_t mov_src_lane_, size_t mov_des_lane_) override;
  void erase_src_des() override;
private:
  // instruction queue
  vector <PUM_ISA> requests;

  // data
  vector <Back_End *> lanes;

  // topology related
  pair <vector <size_t>, vector <size_t> > find_path(size_t src, size_t des);
  bool check_path(vector <size_t> path);
  vector <pair <size_t, size_t> > src_des_pairs;
  vector <size_t> service_timers;
  vector <vector <size_t> > service_paths;
  vector <bool> node_busy;

  // Singal to controller
  bool busy;

  // Performance parameters
  size_t mover_COPY_lat;
  size_t mover_SETUP_lat;

  void service_new_request();
};
#endif

#include "ring_mover.cpp"
