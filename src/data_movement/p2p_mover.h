#ifndef P2P_MOVER_H
#define P2P_MOVER_H

#include <queue>
#include <vector>
#include "./data_mover.h"
#include <utility>

using namespace std;

template <class Back_End>
class P2P_Mover: public Data_Mover<Back_End>
{
public:
  P2P_Mover(size_t mover_COPY_lat_, size_t mover_SETUP_lat_);
  void get_instruction(PUM_ISA entry) override;
  bool is_active() override;
  void tick() override;
  void connect_lanes(vector<Back_End *> lanes_) override;
  void get_src_des(size_t mov_src_lane_, size_t mov_des_lane_) override;
  void erase_src_des() override;
private:
  queue <PUM_ISA> requests;
  vector <Back_End *> lanes;
  vector <pair <size_t, size_t> > src_des_pairs;
  bool busy;
  size_t request_timer;
  size_t setup_timer;
  size_t mover_COPY_lat;
  size_t mover_SETUP_lat;

  void service_new_request();
};
#endif

#include "p2p_mover.cpp"
