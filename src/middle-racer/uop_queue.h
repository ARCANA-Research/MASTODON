#ifndef UOP_QUEUE_H
#define UOP_QUEUE_H
#include <deque>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include "racer_uop.h"
#include "bit_pipeline.h"
#include <sstream>
using namespace std;
class Uop_Queue
{
public:
  // General Info
  size_t pipeline_length;
  size_t queue_size;
  bool system_stall;

  // Member Functions
  Uop_Queue (size_t pipeline_length_, size_t queue_size_);
  int receive_uOp(Racer_Uop uOp);

  bool is_active();

  void receive(vector<size_t> active_regfiles, Racer_Uop uOp);
  void connect_pipelines(vector <Bit_Pipeline *> pipelines);
  void show_log(bool show_);
  void set_log_node(pugi::xml_node parent_node_);
  void record_log(bool record_);
  void toggle_backend(bool disable_signal);
  void write_to_node();
  float report_activity_factor();
  void tick();

private:
  // General Info
  vector <deque < Racer_Uop > > uOp_queues;
  vector <deque < Racer_Uop > > staging; // Verilog sequential registers for the queues
  vector <bool> lock_signals;
  vector <bool> stall_signals;
  vector <size_t> release_counters;
  vector <bool> filter_signals;
  vector <bool> filter_targets;
  size_t stalling_queue;
  vector <Bit_Pipeline *> pipelines;
  vector <Bit_Pipeline *> active_pipelines;

  // Book Keeping
  bool show;
  bool active;
  bool record;
  pugi::xml_node this_node;
  pugi::xml_node parent_node;

  float activity_factor;
  bool backend_disable;

  // Member Functions
  void lazy_copy_when_issue();
  int check_queue_full();
  int check_queue_empty();
  void check_active();
  void pop_fronts();
  void print_uOp_queues();
};

#endif
