#ifndef RACER_DECODER_H
#define RACER_DECODER_H

#include "racer_uop.h"
#include "../top/sst_enable_flag.h"
#include "../top/decoder.h"
#include "../bottom/recipe_table.h"
#include "../data_movement/data_mover.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <vector>
#include "pugixml.hpp"
#if SST_ENABLED
#include "../sst/mpi_server.h"
using namespace Mastodon;
#endif

using namespace std;

class Racer_Decoder : public Decoder<Cluster>
{
public:

// Member Functions
  Racer_Decoder( size_t control_group_ID_,
           size_t num_lane,
           size_t playback_buffer_size_,
           size_t max_num_regfile_per_lane_,
           PUMtech map_style_,
           size_t granularity_);
           
  vector <Cluster *> lanes;
  Racer_Uop uop_to_be_issued;


  
  // override with microarchitecture-specific backend
  string show_uop_to_be_issued() override;
  void connect_lanes(vector <Cluster*> lanes_) override;
  void translate() override;
  bool fetch_torus_evaluation(int lane, int regfile) override;
  bool check_stall() override;
  bool is_connected_backend_active() override;
  void prepare_nop() override;
  void prepare_mask() override;
  void issue_to_backend(size_t lane_idx) override;
};

#endif
