#ifndef SINGLENode_SINGLEINST
#define SINGLENode_SINGLEINST
#include "../../src/top/node.h"
#include "../../src/data_movement/data_mover.h"
#include "../../util/init.h"
#include "../../util/pretty.h"
#include "../../util/config.h"
#include <gtest/gtest.h>
#include "pugixml.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <cstdlib>

TEST(SingleNode_SingleInst, MUL8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] * column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MUL8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, MAC8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column2[i] + column0[i] * column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MAC8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, MUL16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  node.setup(8);
  // node.set_show(true, "controller");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] * column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MUL16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, MUL32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  node.setup(16);
  // node.set_show(true, "controller");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] * column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MUL32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}


TEST(SingleNode_SingleInst, ADD8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/ADD8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}


TEST(SingleNode_SingleInst, ADD16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/ADD16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, ADD32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/ADD32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, ADD64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/ADD64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, INC8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/INC8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, INC16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/INC16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, INC32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/INC32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, INC64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] + 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/INC64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, SUB8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] - column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/SUB8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < output.size(); i+=1) {
    if (column0[i] >= column1[i]) {
        ASSERT_EQ(output[i], expected_output[i]);
    }
  }
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, SUB16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] - column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/SUB16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < output.size(); i+=1) {
    if (column0[i] >= column1[i]) {
        ASSERT_EQ(output[i], expected_output[i]);
    }
  }
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, SUB32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] - column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/SUB32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < output.size(); i+=1) {
    if (column0[i] >= column1[i]) {
        ASSERT_EQ(output[i], expected_output[i]);
    }
  }
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, SUB64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] - column1[i]);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/SUB64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < output.size(); i+=1) {
    if (column0[i] >= column1[i]) {
        ASSERT_EQ(output[i], expected_output[i]);
    }
  }
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CAS8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/CAS8.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output_max;
  vector <size_t> expected_output_min;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output_max.push_back(column0[i]);
      expected_output_min.push_back(column1[i]);
    }
    else {
      expected_output_max.push_back(column1[i]);
      expected_output_min.push_back(column0[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CAS8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output_max = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> output_min = util::fetch_column(node.lanes[0], 0, 3);
  ASSERT_EQ(output_max, expected_output_max);
  ASSERT_EQ(output_min, expected_output_min);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/CAS8.rc");
}

TEST(SingleNode_SingleInst, CAS16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/CAS16.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output_max;
  vector <size_t> expected_output_min;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output_max.push_back(column0[i]);
      expected_output_min.push_back(column1[i]);
    }
    else {
      expected_output_max.push_back(column1[i]);
      expected_output_min.push_back(column0[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CAS16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output_max = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> output_min = util::fetch_column(node.lanes[0], 0, 3);
  ASSERT_EQ(output_max, expected_output_max);
  ASSERT_EQ(output_min, expected_output_min);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/CAS16.rc");
}

TEST(SingleNode_SingleInst, CAS32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/CAS32.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output_max;
  vector <size_t> expected_output_min;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output_max.push_back(column0[i]);
      expected_output_min.push_back(column1[i]);
    }
    else {
      expected_output_max.push_back(column1[i]);
      expected_output_min.push_back(column0[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CAS32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output_max = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> output_min = util::fetch_column(node.lanes[0], 0, 3);
  ASSERT_EQ(output_max, expected_output_max);
  ASSERT_EQ(output_min, expected_output_min);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/CAS32.rc");
}

TEST(SingleNode_SingleInst, CAS64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/CAS64.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output_max;
  vector <size_t> expected_output_min;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output_max.push_back(column0[i]);
      expected_output_min.push_back(column1[i]);
    }
    else {
      expected_output_max.push_back(column1[i]);
      expected_output_min.push_back(column0[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CAS64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output_max = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> output_min = util::fetch_column(node.lanes[0], 0, 3);
  ASSERT_EQ(output_max, expected_output_max);
  ASSERT_EQ(output_min, expected_output_min);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/CAS64.rc");
}

TEST(SingleNode_SingleInst, MAX8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/MAX8.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output.push_back(column0[i]);
    }
    else {
      expected_output.push_back(column1[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MAX8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/MAX8.rc");
}

TEST(SingleNode_SingleInst, MAX16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/MAX16.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output.push_back(column0[i]);
    }
    else {
      expected_output.push_back(column1[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MAX16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/MAX16.rc");
}

TEST(SingleNode_SingleInst, MAX32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/MAX32.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output.push_back(column0[i]);
    }
    else {
      expected_output.push_back(column1[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MAX32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/MAX32.rc");
}

TEST(SingleNode_SingleInst, MAX64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/MAX64.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] >= column1[i]) {
      expected_output.push_back(column0[i]);
    }
    else {
      expected_output.push_back(column1[i]);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MAX64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/MAX64.rc");
}

TEST(SingleNode_SingleInst, MULfixed32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/SingleNode_SingleInst/MULfixed32.erc");

  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
  	expected_output.push_back((column0[i] * column1[i])/16);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/MULfixed32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
    system("rm ./app/SingleNode_SingleInst/MULfixed32.rc");
}

TEST(SingleNode_SingleInst, CMPLT8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] > column1[i]) {
      expected_output.push_back(255);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPLT8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPLT16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] > column1[i]) {
      expected_output.push_back(65535);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPLT16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPLT32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] > column1[i]) {
      expected_output.push_back(4294967295);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPLT32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPLT64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] > column1[i]) {
      expected_output.push_back(-1);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPLT64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPEQ8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] == column1[i]) {
      expected_output.push_back(255);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPEQ8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPEQ16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] == column1[i]) {
      expected_output.push_back(65535);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPEQ16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPEQ32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] == column1[i]) {
      expected_output.push_back(4294967295);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPEQ32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, CMPEQ64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    if (column0[i] == column1[i]) {
      expected_output.push_back(-1);
    }
    else {
      expected_output.push_back(0);
    }
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/CMPEQ64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 2);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, BFLIP8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    size_t reverse_value = 0;
    for (int j = 0; j < 8; j+= 1) {
      reverse_value |= ((column0[i] >> j) & 1) << (7 - j);
    }
    expected_output.push_back(reverse_value);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/BFLIP8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, POPC8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    size_t count = 0;
    // Perform bitwise AND with a mask to consider only the first eight bits
    for (int j = 0; j < 8; j++) {
        if (column0[i] & 1) {
            count++;
        }
        column0[i] >>= 1;
    }
    expected_output.push_back(count);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/POPC8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, LSHIFT8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] << 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/LSHIFT8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, LSHIFT16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] << 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/LSHIFT16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, LSHIFT32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] << 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/LSHIFT32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, LSHIFT64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> expected_output;
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    expected_output.push_back(column0[i] << 1);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/LSHIFT64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  vector <size_t> output = util::fetch_column(node.lanes[0], 0, 1);
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, RDIV8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> output;
  vector <size_t> expected_output;
  vector <int> index;
  // populate expected output
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    // Filter out corner cases
    if (column0[i] < column1[i] || column1[i] > 16 || column1[i] == 0) {
      continue;
    }
    expected_output.push_back(column0[i] % column1[i]);
    index.push_back(i);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/RDIV8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  // populate output
  vector <size_t> column2 = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < index.size(); i+=1) {
    output.push_back(column2[index[i]]);
  }
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, QRDIV8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> quotient;
  vector <size_t> remainder;
  vector <size_t> expected_quotient;
  vector <size_t> expected_remainder;
  vector <int> index;
  // populate expected output
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    // Filter out corner cases
    if (column0[i] < column1[i] || column1[i] > 16 || column1[i] == 0) {
      continue;
    }
    expected_quotient.push_back(column0[i] / column1[i]);
    expected_remainder.push_back(column0[i] % column1[i]);
    index.push_back(i);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/QRDIV8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  // populate output
  column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> column2 = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < index.size(); i+=1) {
    quotient.push_back(column2[index[i]]);
    remainder.push_back(column1[index[i]]);
  }
  ASSERT_EQ(quotient, expected_quotient);
  ASSERT_EQ(remainder, expected_remainder);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}

TEST(SingleNode_SingleInst, QDIV8) {
  srand(0);
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> output;
  vector <size_t> expected_output;
  vector <int> index;
  // populate expected output
  for (unsigned int i = 0; i < column0.size(); i+=1) {
    // Filter out corner cases
    if (column0[i] < column1[i] || column1[i] >= (size_t) 16 || column1[i] == 0 || ((column0[i] / column1[i]) >= (size_t) 16)) {
      continue;
    }
    expected_output.push_back(column0[i] / column1[i]);
    index.push_back(i);
  }
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_SingleInst/QDIV8.rc");
    }
    node.tick();
    
    cout << "Cycle: " << clk << endl;
    if (clk > 50 && clk < 400) {
    	node.set_show(true, "lane0");
    }
    else {
    	node.set_show(false, "all");
    }
    
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    //cout << "\rSimulated Cycles: " << clk << flush;
  }

  // populate output
  vector <size_t> column2 = util::fetch_column(node.lanes[0], 0, 2);
  for (unsigned int i = 0; i < index.size(); i+=1) {
    cout << column0[index[i]] << " / " << column1[index[i]] << "=" << column2[index[i]] << endl;
    output.push_back(column2[index[i]]);
  }
  ASSERT_EQ(output, expected_output);
  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
}
#endif
