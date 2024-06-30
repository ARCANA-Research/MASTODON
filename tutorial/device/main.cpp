#include "../../src/top/node.h"
#include "../../util/init.h"
#include "../../util/pretty.h"
#include "../../util/config.h"
#include "../../src/data_movement/data_mover.h"
#include "pugixml.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>

int main(int argc, char const *argv[]) {
  auto start = std::chrono::high_resolution_clock::now(); 
  // Bookeeping
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  string config(argv[1]);
  // run time variables
  size_t clk = 0;
  size_t MAX_IDLE = stoull(util::general_config_parser("cycle_max_idle", config));

  Node<Cluster, Racer_Decoder> node(0, config);

  // turn on display flags
  node.set_show(true, util::general_config_parser("show", config));


  while (true) 
  {
    std::cout << "\r============== CYCLE:" << clk << " ==============" << flush;;

    // Record logs
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);

    //mpu.lanes[0]->display_data(0);
 
    // load binary
    if (clk == 0) {
      node.load_to_bin(util::general_config_parser("binary", config));
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  
  vector <size_t> A = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> B = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> C = util::fetch_column(node.lanes[0], 0, 2);
  
  // util::pretty_print_cluster(mpu.lanes[0]);
  int total_error = 0;
  int total_NOR = 0;
  double ratio = 0;
  for (unsigned int j = 0; j < node.lanes[0]->pipelines.size(); j+= 1)
  {
    cout << "\nPipeline ID: " << node.lanes[0]->pipelines[j]->ID << endl;
    for (unsigned int i = 0; i < node.lanes[0]->pipelines[0]->tiles.size(); i+= 1) 
    {
      std::pair<int, int> result = node.lanes[0]->pipelines[j]->tiles[i]->report_analog_error();
      total_error += result.first;
      total_NOR += result.second;
    }
  }
  ratio = static_cast<double>(total_error)/total_NOR;
  cout << "Ratio of Incorrect NOR Operations: " << ratio << endl;
  return 0;
}
