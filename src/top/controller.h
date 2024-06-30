#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "sst_enable_flag.h"
#include "data.h"
#include "binary_storage.h"
#include "fetcher.h"
#include "decoder.h"
#include "pum_isa.h"
#include "../data_movement/data_mover.h"
#include <type_traits>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "pugixml.hpp"

#if SST_ENABLED
#include "../sst/mpi_server.h"
using namespace Mastodon;
#endif

using namespace std;

template <class Back_End, class Decoder_Type>
class Controller
{
public:
  // General info
  size_t control_group_ID;
  Fetcher * fetcher;
  Decoder_Type * decoder;
  Data_Mover<Back_End> * mover;
  queue <PUM_ISA> interupt_request_queue;
  string stamp_name;
#if SST_ENABLED
  MPI_Server * mpi_server;
#endif
  // Member functions
  Controller( size_t control_group_ID_,
              size_t num_lane_, // number of cluster/lane
              size_t granularity_,
              size_t playback_buffer_size_, 
              size_t max_num_regfile_per_lane_,
              size_t num_thread_,
              PUMtech map_style_);
  void tick();
  void connect_binary_storage(Binary_Storage* binary_storage_);
  void connect_lanes(vector <Back_End *> lanes_);
  void connect_mover(Data_Mover<Back_End> * mover_);
#if SST_ENABLED
  void connect_mpi_service(MPI_Server * mpi_server_);
#endif
  void show_log(bool show_);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  void write_to_node(string key, string value);
  bool is_backend_active();
  float report_activity_factor();

private:
  // General Info
  size_t PC;
  size_t return_PC;
  Binary_Storage * connected_bin;
  bool check_stall();
  vector <Back_End *> lanes;
  size_t num_lane;
  size_t active_thread_ID;
  size_t num_thread;
  
  size_t outer_most_loop_addr;
  

  //bookeeping
  pugi::xml_node parent_node;
  pugi::xml_node this_node;
  bool show;
  bool record;
};
#endif

#include "controller.cpp"
