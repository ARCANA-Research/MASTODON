#ifndef DECODER_H
#define DECODER_H

#include "sst_enable_flag.h"
#include "pum_isa.h"
#include "../bottom/recipe_table.h"
#include "../data_movement/data_mover.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <deque>
#include <vector>
#include "pugixml.hpp"

#if SST_ENABLED
#include "../sst/mpi_server.h"
using namespace Mastodon;
#endif
using namespace std;

/// Finite State Machine (FSM) of the decoder
enum decoderState {DECODER_READY, ///< The idle state, changed by the Controller
                   DECODER_PREP, ///< The preping stage that fetch the micro-op template or activate the correct lane
                   DECODER_START, ///< Issue the first micro-op to backend, immediately goes to DECODER_CONSTRUCT
                   DECODER_CONSTRUCT, ///< Issue micro-ops to backend, goes to DECODER_READY after done
                   DECODER_FLUSH, ///< Enter a sync stage where the decoder wont accept new instruction until the Uop_Queue is flushed
                   DECODER_LFLUSH,
                   DECODER_WAIT4MOVE, ///< Enter wait stage where decoder wait for the data mover to be done reorganizing data inside the Node
                   DECODER_WAIT4MPI ///< Enter wait stage where decoder wait for the data mover to be done reorganizing data inside the Node
                 };
template <class Back_End>
class Decoder
{
public:
  // General Info
  size_t control_group_ID;
  size_t num_lane;
  // arithGranularity arith_granularity;
  PUMtech map_style;
  decoderState state;

  PUM_ISA * ISA;

  vector <vector <size_t> > active_regfiles; // fixed length
  
  Data_Mover<Back_End> * mover;

#if SST_ENABLED
  MPI_Server * mpi_server;
#endif

  // Member Functions
  Decoder( size_t control_group_ID_,
           size_t num_lane,
           size_t playback_buffer_size_,
           size_t max_num_regfile_per_lane_,
           PUMtech map_style_,
           size_t granularity_);
           
  Decoder(){};

  int get_instruction(string entry, size_t PC); // can modify this function to hold a queue of ISA instead of 1
  void show_log(bool show_);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  //void connect_lanes(vector <Cluster *> lanes_);

  bool is_playback();
  void disable_buffer(bool flag);
  void tick();

//private:
  RecipeTable * recipe_table;
  

  // arithmetic related
  deque <string> arith_template_queue;

  // playback related
  vector <PUM_ISA *> playback_buffer;
  size_t playback_buffer_size;
  size_t max_num_regfile_per_lane;
  size_t playback_PC;
  size_t global_PC_offset;
  bool playback;
  bool early_playback;
  bool buffer_disabled;

  // Member functions
  void prepare_ctrl_flow();
  void prepare_arithmetic();
  void prepare_compute_config();
  void prepare_mover();
  void prepare_cflow();
#if SST_ENABLED
  void prepare_mpi();
#endif
  bool is_valid(size_t addr);
  string vec2str(vector<string> input);
  
  // bookeeping
  bool show;
  bool record;
  
  
  // ITERGRATION NOTICE: OVERRIDE THE FOLLOWING VIRTUAL FUNCTIONS TO CONNECT NEW BACKEND TYPE TO MASTODON
  // Overwrite with microarchitecture-specific backend. General description is as follow:
  // Show case what uop is being issued this cycle
  virtual string show_uop_to_be_issued() = 0;
  // Give the lane pointers to the decoder
  virtual void connect_lanes(vector<Back_End *> lanes) = 0;
  // translate backend related instructions (usually arithmetic or intra-regfile data movement)
  virtual void translate() = 0;
  // fetch the torus register to bring it to the frontend. The torus register is a special register that
  // comparison instructions' results is stored
  virtual bool fetch_torus_evaluation(int lane, int regfile) = 0;
  // check if the active lanes connected to this decoder is stalling (see controller.cpp to see how stalling routine is used)
  virtual bool check_stall() = 0;
  // check if the active lanes connected to this doecder is active. Active means the decoder can still issue new micro-op
  // to the lanes, stalling means no micro-op can be issued. Active lanes can be either active, idle (not active), or stalling (but active)
  virtual bool is_connected_backend_active() = 0;
  // process NOP type instructions
  virtual void prepare_nop() = 0;
  // process predicated intructions
  virtual void prepare_mask() = 0;
  // issue new micro-op to active backends
  virtual void issue_to_backend(size_t lane_idx) = 0;
};

#endif

#include "decoder.cpp"
