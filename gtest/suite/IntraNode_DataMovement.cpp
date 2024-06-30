#ifndef INTRANode_DATAMOVEMENT
#define INTRANode_DATAMOVEMENT
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

TEST(IntraNode_DataMovement, Serial_Single_Same_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_SERIAL.config");
  vector <size_t> regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/single_same_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
  vector <size_t> regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
  vector <size_t> regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
  vector <size_t> regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);

  ASSERT_EQ(regfile0_column0, regfile1_column0);
  ASSERT_EQ(regfile0_column1, regfile1_column1);
  ASSERT_EQ(regfile0_column2, regfile1_column2);
  ASSERT_EQ(regfile0_column3, regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 67, 10);
}

TEST(IntraNode_DataMovement, Serial_Single_Diff_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_SERIAL.config");

  vector <size_t> lane0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/single_diff_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> lane1_column0 = util::fetch_column(node.lanes[1], 0, 0);
  vector <size_t> lane1_column1 = util::fetch_column(node.lanes[1], 0, 1);
  vector <size_t> lane1_column2 = util::fetch_column(node.lanes[1], 0, 2);
  vector <size_t> lane1_column3 = util::fetch_column(node.lanes[1], 0, 3);

  ASSERT_EQ(lane0_column0, lane1_column0);
  ASSERT_EQ(lane0_column1, lane1_column1);
  ASSERT_EQ(lane0_column2, lane1_column2);
  ASSERT_EQ(lane0_column3, lane1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 67, 10);
}

TEST(IntraNode_DataMovement, Serial_Multiple_Diff_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_SERIAL.config");

  vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
  vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
  vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
  vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/multiple_diff_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }


  vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
  vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
  vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
  vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);

  vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
  vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
  vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
  vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);

  ASSERT_EQ(lane0_regfile0_column0, lane1_regfile1_column0);
  ASSERT_EQ(lane0_regfile0_column1, lane1_regfile1_column1);
  ASSERT_EQ(lane0_regfile0_column2, lane1_regfile1_column2);
  ASSERT_EQ(lane0_regfile0_column3, lane1_regfile1_column3);

  ASSERT_EQ(lane2_regfile0_column0, lane3_regfile1_column0);
  ASSERT_EQ(lane2_regfile0_column1, lane3_regfile1_column1);
  ASSERT_EQ(lane2_regfile0_column2, lane3_regfile1_column2);
  ASSERT_EQ(lane2_regfile0_column3, lane3_regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 133, 10);
}

TEST(IntraNode_DataMovement, Serial_Multiple_Same_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_SERIAL.config");

  vector <size_t> lane0_regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
  vector <size_t> lane0_regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
  vector <size_t> lane0_regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
  vector <size_t> lane0_regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);

  vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
  vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
  vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
  vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);

  vector <size_t> lane2_regfile1_column0 = util::fetch_column(node.lanes[2], 1, 0);
  vector <size_t> lane2_regfile1_column1 = util::fetch_column(node.lanes[2], 1, 1);
  vector <size_t> lane2_regfile1_column2 = util::fetch_column(node.lanes[2], 1, 2);
  vector <size_t> lane2_regfile1_column3 = util::fetch_column(node.lanes[2], 1, 3);

  vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
  vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
  vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
  vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/multiple_same_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  vector <size_t> lane1_regfile0_column0 = util::fetch_column(node.lanes[1], 0, 0);
  vector <size_t> lane1_regfile0_column1 = util::fetch_column(node.lanes[1], 0, 1);
  vector <size_t> lane1_regfile0_column2 = util::fetch_column(node.lanes[1], 0, 2);
  vector <size_t> lane1_regfile0_column3 = util::fetch_column(node.lanes[1], 0, 3);

  vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
  vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
  vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
  vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);

  vector <size_t> lane3_regfile0_column0 = util::fetch_column(node.lanes[3], 0, 0);
  vector <size_t> lane3_regfile0_column1 = util::fetch_column(node.lanes[3], 0, 1);
  vector <size_t> lane3_regfile0_column2 = util::fetch_column(node.lanes[3], 0, 2);
  vector <size_t> lane3_regfile0_column3 = util::fetch_column(node.lanes[3], 0, 3);

  ASSERT_EQ(lane0_regfile0_column0, lane0_regfile1_column0);
  ASSERT_EQ(lane0_regfile0_column1, lane0_regfile1_column1);
  ASSERT_EQ(lane0_regfile0_column2, lane0_regfile1_column2);
  ASSERT_EQ(lane0_regfile0_column3, lane0_regfile1_column3);

  ASSERT_EQ(lane1_regfile0_column0, lane1_regfile1_column0);
  ASSERT_EQ(lane1_regfile0_column1, lane1_regfile1_column1);
  ASSERT_EQ(lane1_regfile0_column2, lane1_regfile1_column2);
  ASSERT_EQ(lane1_regfile0_column3, lane1_regfile1_column3);

  ASSERT_EQ(lane2_regfile0_column0, lane2_regfile1_column0);
  ASSERT_EQ(lane2_regfile0_column1, lane2_regfile1_column1);
  ASSERT_EQ(lane2_regfile0_column2, lane2_regfile1_column2);
  ASSERT_EQ(lane2_regfile0_column3, lane2_regfile1_column3);

  ASSERT_EQ(lane3_regfile0_column0, lane3_regfile1_column0);
  ASSERT_EQ(lane3_regfile0_column1, lane3_regfile1_column1);
  ASSERT_EQ(lane3_regfile0_column2, lane3_regfile1_column2);
  ASSERT_EQ(lane3_regfile0_column3, lane3_regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 265, 10);
}
TEST(IntraNode_DataMovement, P2P_Single_Same_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_P2P.config");

  vector <size_t> regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/single_same_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
  vector <size_t> regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
  vector <size_t> regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
  vector <size_t> regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);

  ASSERT_EQ(regfile0_column0, regfile1_column0);
  ASSERT_EQ(regfile0_column1, regfile1_column1);
  ASSERT_EQ(regfile0_column2, regfile1_column2);
  ASSERT_EQ(regfile0_column3, regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 70, 10);
}

TEST(IntraNode_DataMovement, P2P_Single_Diff_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_P2P.config");

  vector <size_t> lane0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/single_diff_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> lane1_column0 = util::fetch_column(node.lanes[1], 0, 0);
  vector <size_t> lane1_column1 = util::fetch_column(node.lanes[1], 0, 1);
  vector <size_t> lane1_column2 = util::fetch_column(node.lanes[1], 0, 2);
  vector <size_t> lane1_column3 = util::fetch_column(node.lanes[1], 0, 3);

  ASSERT_EQ(lane0_column0, lane1_column0);
  ASSERT_EQ(lane0_column1, lane1_column1);
  ASSERT_EQ(lane0_column2, lane1_column2);
  ASSERT_EQ(lane0_column3, lane1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 70, 10);
}

TEST(IntraNode_DataMovement, P2P_Multiple_Diff_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_P2P.config");

  vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
  vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
  vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
  vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/multiple_diff_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }


  vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
  vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
  vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
  vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);

  vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
  vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
  vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
  vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);

  ASSERT_EQ(lane0_regfile0_column0, lane1_regfile1_column0);
  ASSERT_EQ(lane0_regfile0_column1, lane1_regfile1_column1);
  ASSERT_EQ(lane0_regfile0_column2, lane1_regfile1_column2);
  ASSERT_EQ(lane0_regfile0_column3, lane1_regfile1_column3);

  ASSERT_EQ(lane2_regfile0_column0, lane3_regfile1_column0);
  ASSERT_EQ(lane2_regfile0_column1, lane3_regfile1_column1);
  ASSERT_EQ(lane2_regfile0_column2, lane3_regfile1_column2);
  ASSERT_EQ(lane2_regfile0_column3, lane3_regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 70, 10);
}

TEST(IntraNode_DataMovement, P2P_Multiple_Same_Src_Des) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_4l_4r_4x2_P2P.config");

  vector <size_t> lane0_regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
  vector <size_t> lane0_regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
  vector <size_t> lane0_regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
  vector <size_t> lane0_regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);

  vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
  vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
  vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
  vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);

  vector <size_t> lane2_regfile1_column0 = util::fetch_column(node.lanes[2], 1, 0);
  vector <size_t> lane2_regfile1_column1 = util::fetch_column(node.lanes[2], 1, 1);
  vector <size_t> lane2_regfile1_column2 = util::fetch_column(node.lanes[2], 1, 2);
  vector <size_t> lane2_regfile1_column3 = util::fetch_column(node.lanes[2], 1, 3);

  vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
  vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
  vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
  vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/IntraNode_DataMovement/multiple_same_src_des.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }

  vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
  vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);

  vector <size_t> lane1_regfile0_column0 = util::fetch_column(node.lanes[1], 0, 0);
  vector <size_t> lane1_regfile0_column1 = util::fetch_column(node.lanes[1], 0, 1);
  vector <size_t> lane1_regfile0_column2 = util::fetch_column(node.lanes[1], 0, 2);
  vector <size_t> lane1_regfile0_column3 = util::fetch_column(node.lanes[1], 0, 3);

  vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
  vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
  vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
  vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);

  vector <size_t> lane3_regfile0_column0 = util::fetch_column(node.lanes[3], 0, 0);
  vector <size_t> lane3_regfile0_column1 = util::fetch_column(node.lanes[3], 0, 1);
  vector <size_t> lane3_regfile0_column2 = util::fetch_column(node.lanes[3], 0, 2);
  vector <size_t> lane3_regfile0_column3 = util::fetch_column(node.lanes[3], 0, 3);

  ASSERT_EQ(lane0_regfile0_column0, lane0_regfile1_column0);
  ASSERT_EQ(lane0_regfile0_column1, lane0_regfile1_column1);
  ASSERT_EQ(lane0_regfile0_column2, lane0_regfile1_column2);
  ASSERT_EQ(lane0_regfile0_column3, lane0_regfile1_column3);

  ASSERT_EQ(lane1_regfile0_column0, lane1_regfile1_column0);
  ASSERT_EQ(lane1_regfile0_column1, lane1_regfile1_column1);
  ASSERT_EQ(lane1_regfile0_column2, lane1_regfile1_column2);
  ASSERT_EQ(lane1_regfile0_column3, lane1_regfile1_column3);

  ASSERT_EQ(lane2_regfile0_column0, lane2_regfile1_column0);
  ASSERT_EQ(lane2_regfile0_column1, lane2_regfile1_column1);
  ASSERT_EQ(lane2_regfile0_column2, lane2_regfile1_column2);
  ASSERT_EQ(lane2_regfile0_column3, lane2_regfile1_column3);

  ASSERT_EQ(lane3_regfile0_column0, lane3_regfile1_column0);
  ASSERT_EQ(lane3_regfile0_column1, lane3_regfile1_column1);
  ASSERT_EQ(lane3_regfile0_column2, lane3_regfile1_column2);
  ASSERT_EQ(lane3_regfile0_column3, lane3_regfile1_column3);

  cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
  EXPECT_NEAR(clk - MAX_IDLE, 70, 10);
}

// TEST(IntraNode_DataMovement, Ring_Single_Same_Src_Des) {
//   size_t clk = 0;
//   size_t MAX_IDLE = 100;
//   Node<Cluster, Racer_Decoder> node(0, "./config/SingleNode<Cluster, Racer_Decoder>_8b_4l_4r_RingMOV.config");
//
//   vector <size_t> regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
//   vector <size_t> regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
//   vector <size_t> regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
//   vector <size_t> regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);
//
//   while (true) {
//     // load binary
//     if (clk == 0) {
//       node.load_to_bin("./app/IntraNode_DataMovement/single_same_src_des.rc");
//     }
//     node.tick();
//     if (node.idle_counter == MAX_IDLE) {
//       break;
//     }
//     clk ++;
//   }
//
//   vector <size_t> regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
//   vector <size_t> regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
//   vector <size_t> regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
//   vector <size_t> regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);
//
//   ASSERT_EQ(regfile0_column0, regfile1_column0);
//   ASSERT_EQ(regfile0_column1, regfile1_column1);
//   ASSERT_EQ(regfile0_column2, regfile1_column2);
//   ASSERT_EQ(regfile0_column3, regfile1_column3);
//
//   cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
// }
//
// TEST(IntraNode_DataMovement, Ring_Single_Diff_Src_Des) {
//   size_t clk = 0;
//   size_t MAX_IDLE = 100;
//   Node<Cluster, Racer_Decoder> node(0, "./config/SingleNode<Cluster, Racer_Decoder>_8b_4l_4r_RingMOV.config");
//
//   vector <size_t> lane0_column0 = util::fetch_column(node.lanes[0], 0, 0);
//   vector <size_t> lane0_column1 = util::fetch_column(node.lanes[0], 0, 1);
//   vector <size_t> lane0_column2 = util::fetch_column(node.lanes[0], 0, 2);
//   vector <size_t> lane0_column3 = util::fetch_column(node.lanes[0], 0, 3);
//
//   while (true) {
//     // load binary
//     if (clk == 0) {
//       node.load_to_bin("./app/IntraNode_DataMovement/single_diff_src_des.rc");
//     }
//     node.tick();
//     if (node.idle_counter == MAX_IDLE) {
//       break;
//     }
//     clk ++;
//   }
//
//   vector <size_t> lane1_column0 = util::fetch_column(node.lanes[1], 0, 0);
//   vector <size_t> lane1_column1 = util::fetch_column(node.lanes[1], 0, 1);
//   vector <size_t> lane1_column2 = util::fetch_column(node.lanes[1], 0, 2);
//   vector <size_t> lane1_column3 = util::fetch_column(node.lanes[1], 0, 3);
//
//   ASSERT_EQ(lane0_column0, lane1_column0);
//   ASSERT_EQ(lane0_column1, lane1_column1);
//   ASSERT_EQ(lane0_column2, lane1_column2);
//   ASSERT_EQ(lane0_column3, lane1_column3);
//
//   cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
// }
//
// TEST(IntraNode_DataMovement, Ring_Multiple_Diff_Src_Des) {
//   size_t clk = 0;
//   size_t MAX_IDLE = 100;
//   Node<Cluster, Racer_Decoder> node(0, "./config/SingleNode<Cluster, Racer_Decoder>_8b_4l_4r_RingMOV.config");
//
//   vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
//   vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
//   vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
//   vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);
//
//   vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
//   vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
//   vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
//   vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);
//
//   while (true) {
//     // load binary
//     if (clk == 0) {
//       node.load_to_bin("./app/IntraNode_DataMovement/multiple_diff_src_des.rc");
//     }
//     node.tick();
//     if (node.idle_counter == MAX_IDLE) {
//       break;
//     }
//     clk ++;
//   }
//
//
//   vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
//   vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
//   vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
//   vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);
//
//   vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
//   vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
//   vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
//   vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);
//
//   ASSERT_EQ(lane0_regfile0_column0, lane1_regfile1_column0);
//   ASSERT_EQ(lane0_regfile0_column1, lane1_regfile1_column1);
//   ASSERT_EQ(lane0_regfile0_column2, lane1_regfile1_column2);
//   ASSERT_EQ(lane0_regfile0_column3, lane1_regfile1_column3);
//
//   ASSERT_EQ(lane2_regfile0_column0, lane3_regfile1_column0);
//   ASSERT_EQ(lane2_regfile0_column1, lane3_regfile1_column1);
//   ASSERT_EQ(lane2_regfile0_column2, lane3_regfile1_column2);
//   ASSERT_EQ(lane2_regfile0_column3, lane3_regfile1_column3);
//
//   cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
// }
//
// TEST(IntraNode_DataMovement, Ring_Multiple_Same_Src_Des) {
//   size_t clk = 0;
//   size_t MAX_IDLE = 100;
//   Node<Cluster, Racer_Decoder> node(0, "./config/SingleNode<Cluster, Racer_Decoder>_8b_4l_4r_RingMOV.config");
//
//   vector <size_t> lane0_regfile0_column0 = util::fetch_column(node.lanes[0], 0, 0);
//   vector <size_t> lane0_regfile0_column1 = util::fetch_column(node.lanes[0], 0, 1);
//   vector <size_t> lane0_regfile0_column2 = util::fetch_column(node.lanes[0], 0, 2);
//   vector <size_t> lane0_regfile0_column3 = util::fetch_column(node.lanes[0], 0, 3);
//
//   vector <size_t> lane1_regfile0_column0 = util::fetch_column(node.lanes[1], 0, 0);
//   vector <size_t> lane1_regfile0_column1 = util::fetch_column(node.lanes[1], 0, 1);
//   vector <size_t> lane1_regfile0_column2 = util::fetch_column(node.lanes[1], 0, 2);
//   vector <size_t> lane1_regfile0_column3 = util::fetch_column(node.lanes[1], 0, 3);
//
//   vector <size_t> lane2_regfile0_column0 = util::fetch_column(node.lanes[2], 0, 0);
//   vector <size_t> lane2_regfile0_column1 = util::fetch_column(node.lanes[2], 0, 1);
//   vector <size_t> lane2_regfile0_column2 = util::fetch_column(node.lanes[2], 0, 2);
//   vector <size_t> lane2_regfile0_column3 = util::fetch_column(node.lanes[2], 0, 3);
//
//   vector <size_t> lane3_regfile0_column0 = util::fetch_column(node.lanes[3], 0, 0);
//   vector <size_t> lane3_regfile0_column1 = util::fetch_column(node.lanes[3], 0, 1);
//   vector <size_t> lane3_regfile0_column2 = util::fetch_column(node.lanes[3], 0, 2);
//   vector <size_t> lane3_regfile0_column3 = util::fetch_column(node.lanes[3], 0, 3);
//
//   while (true) {
//     // load binary
//     if (clk == 0) {
//       node.load_to_bin("./app/IntraNode_DataMovement/multiple_same_src_des.rc");
//     }
//     node.tick();
//     if (node.idle_counter == MAX_IDLE) {
//       break;
//     }
//     clk ++;
//   }
//
//   vector <size_t> lane0_regfile1_column0 = util::fetch_column(node.lanes[0], 1, 0);
//   vector <size_t> lane0_regfile1_column1 = util::fetch_column(node.lanes[0], 1, 1);
//   vector <size_t> lane0_regfile1_column2 = util::fetch_column(node.lanes[0], 1, 2);
//   vector <size_t> lane0_regfile1_column3 = util::fetch_column(node.lanes[0], 1, 3);
//
//   vector <size_t> lane1_regfile1_column0 = util::fetch_column(node.lanes[1], 1, 0);
//   vector <size_t> lane1_regfile1_column1 = util::fetch_column(node.lanes[1], 1, 1);
//   vector <size_t> lane1_regfile1_column2 = util::fetch_column(node.lanes[1], 1, 2);
//   vector <size_t> lane1_regfile1_column3 = util::fetch_column(node.lanes[1], 1, 3);
//
//   vector <size_t> lane2_regfile1_column0 = util::fetch_column(node.lanes[2], 1, 0);
//   vector <size_t> lane2_regfile1_column1 = util::fetch_column(node.lanes[2], 1, 1);
//   vector <size_t> lane2_regfile1_column2 = util::fetch_column(node.lanes[2], 1, 2);
//   vector <size_t> lane2_regfile1_column3 = util::fetch_column(node.lanes[2], 1, 3);
//
//   vector <size_t> lane3_regfile1_column0 = util::fetch_column(node.lanes[3], 1, 0);
//   vector <size_t> lane3_regfile1_column1 = util::fetch_column(node.lanes[3], 1, 1);
//   vector <size_t> lane3_regfile1_column2 = util::fetch_column(node.lanes[3], 1, 2);
//   vector <size_t> lane3_regfile1_column3 = util::fetch_column(node.lanes[3], 1, 3);
//
//   ASSERT_EQ(lane0_regfile0_column0, lane0_regfile1_column0);
//   ASSERT_EQ(lane0_regfile0_column1, lane0_regfile1_column1);
//   ASSERT_EQ(lane0_regfile0_column2, lane0_regfile1_column2);
//   ASSERT_EQ(lane0_regfile0_column3, lane0_regfile1_column3);
//
//   ASSERT_EQ(lane1_regfile0_column0, lane1_regfile1_column0);
//   ASSERT_EQ(lane1_regfile0_column1, lane1_regfile1_column1);
//   ASSERT_EQ(lane1_regfile0_column2, lane1_regfile1_column2);
//   ASSERT_EQ(lane1_regfile0_column3, lane1_regfile1_column3);
//
//   ASSERT_EQ(lane2_regfile0_column0, lane2_regfile1_column0);
//   ASSERT_EQ(lane2_regfile0_column1, lane2_regfile1_column1);
//   ASSERT_EQ(lane2_regfile0_column2, lane2_regfile1_column2);
//   ASSERT_EQ(lane2_regfile0_column3, lane2_regfile1_column3);
//
//   ASSERT_EQ(lane3_regfile0_column0, lane3_regfile1_column0);
//   ASSERT_EQ(lane3_regfile0_column1, lane3_regfile1_column1);
//   ASSERT_EQ(lane3_regfile0_column2, lane3_regfile1_column2);
//   ASSERT_EQ(lane3_regfile0_column3, lane3_regfile1_column3);
//
//   cout << "Simulated Cycles: " << clk - MAX_IDLE << endl;
// }
#endif
