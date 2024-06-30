#include "../src/top/sst_enable_flag.h"
#include "../src/top/node.h"
#include "../src/data_movement/data_mover.h"
#include "../util/init.h"
#include "../util/pretty.h"
#include "../util/config.h"
#include <gtest/gtest.h>
#include "pugixml.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>

int main(int argc, char ** argv) {
  std::srand(std::time(0));
  testing::InitGoogleTest(&argc, argv);

  // Specify the number of trials
  int numTrials = 1;

  // Run the tests multiple times
  for (int trial = 0; trial < numTrials; ++trial) {
      std::cout << "================= Random Trial " << (trial + 1) << "=================" << std::endl;
      int result = RUN_ALL_TESTS();
      if (result != 0) {
          return result;
      }
  }
}
