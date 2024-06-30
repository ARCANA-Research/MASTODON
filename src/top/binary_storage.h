#ifndef BINARY_STORAGE_H
#define BINARY_STORAGE_H
#include "data.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "pugixml.hpp"
#include <string>

using namespace std;

class Binary_Storage
{
public:
  // General Info
  size_t ID;
  size_t output_latency; // latency (cycle) to read out off bin storage
  size_t input_latency; // latency (cycle) to write into bin storage
  size_t data_size;  // storage entry size
  size_t total_entries;  // total number of entries
  size_t num_free_entries; // number of free entries

  // Load Related
  bool output_busy; // if true, stop reading binary (port busy)

  // Store Related
  bool input_busy; // if true, stop accepting new binary (port busy)

  bool delete_busy; // if true, the current cycle is spent deleting entries

  // Member Functions
  Binary_Storage(size_t ID, size_t total_entries_, size_t output_latency_, size_t input_latency_, size_t data_size_);
  void tick();
  int store(char* binary_file, size_t file_size);
  int delete_entry(size_t delete_size);
  int send_to_controller(size_t pc, char * entry);
  void show_log(bool show_);
  void record_log(bool record_);
  void set_log_node(pugi::xml_node parent_node_);
  void write_to_node(char * binary_entry);
  void print_storage();

private:
  // General Info
  unsigned long long int time;  // local time, time = 0 at bin storage instantiation
  vector< Data<char> > stored_data;

  // Load Related

  // Store Related
  size_t input_start_idx; // the starting position in the stored_data vector where entries are loaded into
  size_t input_start_time;  // the timestamp when store begins
  unsigned long long int input_done_time; // time stamp when current store will be finished

  pugi::xml_node parent_node;
  pugi::xml_node this_node;

  // Bookeeping
  bool show;
  bool record;
};
#endif
