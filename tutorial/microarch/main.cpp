#include "../../src/top/node.h"
#include "../../util/init.h"
#include "../../util/pretty.h"
#include "../../util/config.h"
#include <iostream>
#include <vector>
#include <sstream>

int main(int argc, char const *argv[]) {
  string config_file = argv[1];
  // Instantiate a mastodon node
  Node<Cluster, Racer_Decoder> node(0, config_file);
  
  // Instantiate runtime variables 
  size_t clk = 0;
  size_t max_idle = stoull(util::general_config_parser("cycle_max_idle", config_file));
  string microkernel = util::general_config_parser("binary", config_file);

  // Turn on display flags
  string show_config = util::general_config_parser("show", config_file);
  cout << show_config << endl;
  node.set_show(true, show_config);
  
  // Optionally load user-defined data into lanes of the node
  string data_source = util::general_config_parser("data_source", config_file);
  if (data_source != "none") {
  	util::read_data_from_file(node.lanes, data_source);
  }

  // Simulation main loop
  while (true) {
    if (show_config != "none") {cout << "============== CYCLE:" << clk << " ==============" << endl;}
    // Load binary
    if (clk == 0) {
      node.load_to_bin(microkernel);
    }
    // Increment the clock
    node.tick();
    if (node.idle_counter == max_idle) {
      break;
    }
    clk ++;
  }
  
  // For ADD.rc demonstration
  vector <size_t> A = util::fetch_column(node.lanes[0],0, 1); // fetch register from lane, reg.file, reg addr
  vector <size_t> B = util::fetch_column(node.lanes[0],0, 2);
  vector <size_t> C = util::fetch_column(node.lanes[0],0, 0);

  for (auto i = 0; i < A.size(); i += 1) {
    cout << A[i] << " + " << B[i] << " = " << C[i] << endl;
  }


  // Perform final report
  util::report_microkernel_node(&node, clk - max_idle, config_file);
  
  
  // For DFT demonstration
  return 0;
}
