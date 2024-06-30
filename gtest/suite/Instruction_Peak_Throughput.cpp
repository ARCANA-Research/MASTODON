#ifndef INSTRUCTION_PEAK_THROUGHPUT
#define INSTRUCTION_PEAK_THROUGHPUT
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

TEST(Instruction_Peak_Throughput, MUL8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  vector <size_t> column0 = util::fetch_column(node.lanes[0], 0, 0);
  vector <size_t> column1 = util::fetch_column(node.lanes[0], 0, 1);
  vector <size_t> expected_output;

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MUL8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, MAC8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MAC8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CAS8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/CAS8.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CAS8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/CAS8.rc");
}

TEST(Instruction_Peak_Throughput, CAS16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/CAS16.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CAS16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/CAS16.rc");
}


TEST(Instruction_Peak_Throughput, CAS32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/CAS32.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CAS32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/CAS32.rc");
}


TEST(Instruction_Peak_Throughput, CAS64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/CAS64.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CAS64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/CAS64.rc");
}



TEST(Instruction_Peak_Throughput, MAX8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/MAX8.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MAX8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/MAX8.rc");
}

TEST(Instruction_Peak_Throughput, MAX16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/MAX16.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MAX16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/MAX16.rc");
}

TEST(Instruction_Peak_Throughput, MAX32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/MAX32.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MAX32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/MAX32.rc");
}

TEST(Instruction_Peak_Throughput, MAX64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");
  system("python3 ../util/ezpim.py ./app/Instruction_Peak_Throughput/MAX64.erc");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MAX64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
  system("rm ./app/Instruction_Peak_Throughput/MAX64.rc");
}

TEST(Instruction_Peak_Throughput, MUX8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MUX8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, MUX16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MUX16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, MUX32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MUX32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, MUX64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/MUX64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, RELU8v1) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");
  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/RELU8v1.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, RELU16v1) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/RELU16v1.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, RELU32v1) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/RELU32v1.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, RELU64v1) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/RELU64v1.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, ADD8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/ADD8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, ADD16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/ADD16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, ADD32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/ADD32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, ADD64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/ADD64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, SUB8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/SUB8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, SUB16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/SUB16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, SUB32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/SUB32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, SUB64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/SUB64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPLT8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPLT8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPLT16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPLT16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPLT32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPLT32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPLT64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPLT64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, FUZZY8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/FUZZY8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, FUZZY16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/FUZZY16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, FUZZY32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/FUZZY32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, FUZZY64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/FUZZY64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPEQ8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPEQ8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPEQ16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPEQ16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPEQ32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPEQ32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, CMPEQ64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/CMPEQ64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, BFLIP8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/BFLIP8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, POPC8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/POPC8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, LSHIFT8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/LSHIFT8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, LSHIFT16) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/16b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/LSHIFT16.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/16b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, LSHIFT32) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/LSHIFT32.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/32b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, LSHIFT64) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/64b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/LSHIFT64.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/64b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, RDIV8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/RDIV8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, QRDIV8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/QRDIV8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}

TEST(Instruction_Peak_Throughput, QDIV8) {
  size_t clk = 0;
  size_t MAX_IDLE = 100;
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_1l_4r_64x64_SERIAL.config");

  while (true) {
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/Instruction_Peak_Throughput/QDIV8.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
  }
  SUCCEED();

  util::report_peak_node(&node, clk - MAX_IDLE, "./config/8b_1l_4r_64x64_SERIAL.config", 100);
}
#endif
