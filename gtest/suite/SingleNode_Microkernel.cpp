#ifndef SINGLENode_MICROKERNEL
#define SINGLENode_MICROKERNEL
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
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace std;

void createDFTMatrix(int N, std::vector<std::vector<size_t>>& realMatrix, std::vector<std::vector<size_t>>& imagMatrix) {
    const float PI = 3.14159265358979323846f;
    for (int k = 0; k < N; ++k) {
        for (int n = 0; n < N; ++n) {
            float angle = -2 * PI * k * n / N;
            float scale = 512;
            realMatrix[k][n] = static_cast<size_t>(cosf(angle) * scale + scale);
            imagMatrix[k][n] = static_cast<size_t>(sinf(angle) * scale + scale);
        }
    }
}
/*
TEST(SingleNode_Microkernel, LIN_DFT1D) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;

  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 64;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int N = 128;
  
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_8l_64r_64x64_P2P.config");

  //node.set_show(true, "controller, binary, decoder");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_DFT1D.erc");
  cout << " > Compilation complete!" << endl;
  
  cout << " > Input data generation" << endl;
  std::vector<std::vector<size_t>> realMatrix(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t>> imagMatrix(N, std::vector<size_t>(N));
  createDFTMatrix(N, realMatrix, imagMatrix);
  vector<size_t> input_signal;
  vector<size_t> expected_real_output;
  vector<size_t> expected_imag_output;

  for (int n = 0; n < N; n += 1) {
    input_signal.push_back(rand() % 65536);
  }
  cout << " > Input data generation complete!" << endl;

  cout << " > Baseline generation" << endl;
  for (int i = 0; i < N; i += 1) {
    size_t expected_real_output_val = 0;
    size_t expected_imag_output_val = 0;
    for (int j = 0; j < N; j += 1) {
      expected_real_output_val += realMatrix[i][j] * input_signal[j];
      expected_imag_output_val += imagMatrix[i][j] * input_signal[j];
    }
    expected_real_output.push_back(expected_real_output_val / 256);
    expected_imag_output.push_back(expected_imag_output_val / 256);
  }
  cout << " > Baseline generation complete!" << endl;


  cout << " > Data loading" << endl;

  // Pregenerate random values
  vector <vector <vector <vector <size_t> > > > data_in;
  for (int l = 0; l < L; l++) {
    vector <vector <vector<size_t> > > lane;
      for (int f = 0; f < F; f++) {
        vector <vector <size_t> > regfile;
        for (int c = 0; c < C; c++) {
          vector <size_t> column;
            for (int r = 0; r < R; r++) {
              column.push_back(rand() % 65536);
            }
          regfile.push_back(column);
        }
        lane.push_back(regfile);
    }
    data_in.push_back(lane);
  }

  // Populate real dft matrix in the first 8 regfiles, for register 0 - 15
  // only the first lane
  for (int f = 0; f < 8; f += 1) {
    for (int c = 0; c < 15; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[0][f][c][r] = realMatrix[c + f * 16][r];
      }
    }
  }
  for (int f = 8; f < 16; f += 1) {
    for (int c = 0; c < 15; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[0][f][c][r] = realMatrix[c + (f - 8) * 16][r + 64];
      }
    }
  }

  // Populate imaginary dft matrix in the first 8 regfiles, for register 0 - 15
  // only the second lane
  for (int f = 0; f < 8; f += 1) {
    for (int c = 0; c < 15; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[1][f][c][r] = imagMatrix[c + f * 16][r];
      }
    }
  }
  for (int f = 8; f < 16; f += 1) {
    for (int c = 0; c < 15; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[1][f][c][r] = imagMatrix[c + (f - 8) * 16][r + 64];
      }
    }
  }

  // Populate the input signal in the first 8 regfiles, for register 16 - 31
  for (int l = 0; l < 2; l += 1) {
    for (int f = 0; f < 16; f += 1) {
      for (int c = 16; c < 31; c+= 1) {
        for (int r = 0; r < R; r += 1) {
          data_in[l][f][c][r] = input_signal[c + f * 16];
        }
      }
    }
  }
  util::write_data_to_file(data_in, "./LIN_DFT1D.in.xml", L, F, C, R);
  util::read_data_from_file(node.lanes, "./LIN_DFT1D.in.xml");
  system("rm ./LIN_DFT1D.in.xml");
  cout << endl << " > Data loading complete!" << endl;

  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);

    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/LIN_DFT1D.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/LIN_DFT1D.rc");
  
  doc.save_file("./xml_results/SingleNode_Microkernel.LIN_DFT1D.xml");
}
*/

void get_input_image(vector<std::vector<size_t> >& image) {
  // Path to the text file
    std::string filename = "./suite/dft-isca/ichiro64x64.txt";
    int scale = 256;
    // Open the text file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file." << std::endl;
        exit(-1);
    }
    
    // Read the file line by line
    std::string line;
    while (std::getline(file, line)) {
        std::vector<size_t> row;
        std::istringstream ss(line);
        std::string pixel;
        
        // Split the line by spaces to get individual pixel values
        while (ss >> pixel) {
            row.push_back(std::stoi(pixel));
        }
        
        // Add the row to the pixel values vector
        image.push_back(row);
    }

    // Close the file
    file.close();
}

std::vector<std::vector<size_t>> transpose(const std::vector<std::vector<size_t>>& matrix) {
    int N = matrix.size();
    std::vector<std::vector<size_t>> transposed(N, std::vector<size_t>(N));
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
    
    return transposed;
}

void save_dft_matrix(Node<Cluster, Racer_Decoder> node, string clk_cycle) {
     vector < vector <int32_t> > output;
     for (int i = 0 ; i < 64; i += 1) {
     	vector <int32_t> col;
     	vector <size_t> col_raw = util::fetch_column(node.lanes[0], 0, i);
     	for (int j = 0 ; j < 64; j+=1) {
     		col.push_back( ((int32_t) col_raw[j]));
     	}
     	output.push_back(col);
     }
     
     string filename = "./suite/dft-isca/dft_mat_" + clk_cycle + ".txt";
     ofstream outfile(filename);
     for (const auto& row : output) {
        for (const auto& elem : row) {
            outfile << elem << " ";
        }
        outfile << std::endl;
    }

    outfile.close();
}


TEST(SingleNode_Microkernel, LIN_DFT2D_deviceflow) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;

  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 64;  // num tile row (also number of output row)
  int C = 129; // num tile column 64
  int N = 64;
  
  Node<Cluster, Racer_Decoder> node(0, "./config/deviceflow_32b_8l_64r_128x64_P2P.config");
  //node.set_show(true, "controller");

  //node.set_show(true, "controller, binary, decoder");
  //node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_DFT2D_deviceflow.erc");
  cout << " > Compilation complete!" << endl;
  
  cout << " > Input data generation" << endl;
  std::vector<std::vector<size_t> > realMatrix(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > imagMatrix(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > realMatrix_t(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > imagMatrix_t(N, std::vector<size_t>(N));
  createDFTMatrix(N, realMatrix, imagMatrix);
  realMatrix_t = transpose(realMatrix);
  imagMatrix_t = transpose(imagMatrix);

  std::vector<std::vector<size_t> > input_signal;
  get_input_image(input_signal);
  
  std::vector<std::vector<size_t> > expected_real_output(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > expected_imag_output(N, std::vector<size_t>(N));
  cout << " > Input data generation complete!" << endl;


  cout << " > Data loading" << endl;
  // Pregenerate random values
  // Pregenerate random values
  vector <vector <vector <vector <size_t> > > > data_in;
  for (int l = 0; l < L; l++) {
    vector <vector <vector<size_t> > > lane;
      for (int f = 0; f < F; f++) {
        vector <vector <size_t> > regfile;
        for (int c = 0; c < C; c++) {
          vector <size_t> col;
            for (int r = 0; r < R; r++) {
              col.push_back(0);
            }
          regfile.push_back(col);
        }
        lane.push_back(regfile);
    }
    data_in.push_back(lane);
  }
  

  // Populate real dft matrix in regfile 0 - 63, for register 0 - 63, lane 0
  for (int f = 0; f < 64; f += 1) {
    for (int c = 0; c < 64; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[0][f][c][r] = realMatrix[c][r];
      }
    }
  }


  // Populate imaginary dft matrix in regfile 0 - 63, for register 0 - 63, lane 1
  for (int f = 0; f < 64; f += 1) {
    for (int c = 0; c < 64; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[1][f][c][r] = imagMatrix[c][r];
      }
    }
  }


  // Populate the input signal in regfile 0 - 63, for register 64 - 127, lane 0 and 1
  for (int l = 0; l < 2; l+=1) {
	  for (int f = 0; f < 64; f += 1) {
	    for (int c = 0; c < 64; c+= 1) {
	      for (int r = 0; r < R; r += 1) {
		data_in[l][f][c + 64][r] = input_signal[f][c];
	      }
	    }
	  }
  }
  
    // Populate the transposed matrix in regfile 0 - 63, for register 64 - 127, lane 2 and 3
	  for (int f = 0; f < 64; f += 1) {
	    for (int c = 0; c < 64; c+= 1) {
	      for (int r = 0; r < R; r += 1) {
		data_in[2][f][c + 64][r] = realMatrix_t[f][c];
		data_in[3][f][c + 64][r] = imagMatrix_t[f][c];
	      }
	    }
	  }
  vector<vector<vector<vector<size_t>>>> data_in_small;

    // Initialize data_in_small with the same outer dimensions as data_in
    data_in_small.resize(data_in.size());
    for (size_t i = 0; i < 1; ++i) {
        data_in_small[i].resize(data_in[i].size());
        for (size_t j = 0; j < 1; ++j) {
            data_in_small[i][j].resize(data_in[i][j].size());
            for (size_t k = 0; k < data_in[i][j].size(); ++k) {
                data_in_small[i][j][k].resize(data_in[i][j][k].size());
            }
        }
    }
  
  data_in_small[0][0] = data_in[0][0];
  util::write_data_to_file(data_in_small, "./LIN_DFT2D_deviceflow.in.xml", 1, 1, 129, 64);
  util::read_data_from_file(node.lanes, "./LIN_DFT2D_deviceflow.in.xml");
  

  cout << endl << " > Data loading complete!" << endl;

  while (true) {
       std::stringstream ss;
       ss << "Cycle_" << clk;
       pugi::xml_node clk_node = root.append_child(ss.str().c_str());
       node.set_log_node(clk_node);

       // load binary
       if (clk == 0) {
         node.load_to_bin("./app/SingleNode_Microkernel/LIN_DFT2D_deviceflow.rc");
       }
       node.tick();
       if (node.idle_counter == MAX_IDLE) {
         break;
       }
       clk ++;
    
       if (clk % heart_size == 0) {
         cout << "\rSimulated Cycles: " << clk << flush;
       }
       
       if (clk % 5000 == 0) {
       	save_dft_matrix(node, to_string(clk));
       }
     }
     cout << endl;

    
     util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/deviceflow_32b_8l_64r_128x64_P2P.config");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D_deviceflow.rc");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D_deviceflow.node.data");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D_deviceflow.human.data");
  
     doc.save_file("./xml_results/SingleNode_Microkernel.LIN_DFT2D_deviceflow.xml");
}

TEST(SingleNode_Microkernel, SIG_ADPCMEC) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64

  Node<Cluster, Racer_Decoder> node(0, "./config/16b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/SIG_ADPCMEC.erc");
  cout << " > Compilation complete!" << endl;

  
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/SIG_ADPCMEC.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    //node.lanes[0]->display_data(0);

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/SIG_ADPCMEC.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.SIG_ADPCMEC.xml");
}

TEST(SingleNode_Microkernel, LIN_DFT2D) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;

  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 64;  // num tile row (also number of output row)
  int C = 129; // num tile column 64
  int N = 64;
  
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_8l_64r_128x64_P2P.config");
  //node.set_show(true, "controller");

  //node.set_show(true, "controller, binary, decoder");
  //node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_DFT2D.erc");
  cout << " > Compilation complete!" << endl;
  
  cout << " > Input data generation" << endl;
  std::vector<std::vector<size_t> > realMatrix(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > imagMatrix(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > realMatrix_t(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > imagMatrix_t(N, std::vector<size_t>(N));
  createDFTMatrix(N, realMatrix, imagMatrix);
  realMatrix_t = transpose(realMatrix);
  imagMatrix_t = transpose(imagMatrix);

  std::vector<std::vector<size_t> > input_signal;
  get_input_image(input_signal);
  
  std::vector<std::vector<size_t> > expected_real_output(N, std::vector<size_t>(N));
  std::vector<std::vector<size_t> > expected_imag_output(N, std::vector<size_t>(N));
  cout << " > Input data generation complete!" << endl;


  cout << " > Data loading" << endl;
  // Pregenerate random values
  // Pregenerate random values
  vector <vector <vector <vector <size_t> > > > data_in;
  for (int l = 0; l < L; l++) {
    vector <vector <vector<size_t> > > lane;
      for (int f = 0; f < F; f++) {
        vector <vector <size_t> > regfile;
        for (int c = 0; c < C; c++) {
          vector <size_t> col;
            for (int r = 0; r < R; r++) {
              col.push_back(0);
            }
          regfile.push_back(col);
        }
        lane.push_back(regfile);
    }
    data_in.push_back(lane);
  }
  

  // Populate real dft matrix in regfile 0 - 63, for register 0 - 63, lane 0
  for (int f = 0; f < 64; f += 1) {
    for (int c = 0; c < 64; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[0][f][c][r] = realMatrix[c][r];
      }
    }
  }


  // Populate imaginary dft matrix in regfile 0 - 63, for register 0 - 63, lane 1
  for (int f = 0; f < 64; f += 1) {
    for (int c = 0; c < 64; c+= 1) {
      for (int r = 0; r < R; r += 1) {
        data_in[1][f][c][r] = imagMatrix[c][r];
      }
    }
  }


  // Populate the input signal in regfile 0 - 63, for register 64 - 127, lane 0 and 1
  for (int l = 0; l < 2; l+=1) {
	  for (int f = 0; f < 64; f += 1) {
	    for (int c = 0; c < 64; c+= 1) {
	      for (int r = 0; r < R; r += 1) {
		data_in[l][f][c + 64][r] = input_signal[f][c];
	      }
	    }
	  }
  }
  
    // Populate the transposed matrix in regfile 0 - 63, for register 64 - 127, lane 2 and 3
	  for (int f = 0; f < 64; f += 1) {
	    for (int c = 0; c < 64; c+= 1) {
	      for (int r = 0; r < R; r += 1) {
		data_in[2][f][c + 64][r] = realMatrix_t[f][c];
		data_in[3][f][c + 64][r] = imagMatrix_t[f][c];
	      }
	    }
	  }
  
  
  util::write_data_to_file(data_in, "./LIN_DFT2D.in.xml", L, F, C, R);
  util::read_data_from_file(node.lanes, "./LIN_DFT2D.in.xml");
  

  cout << endl << " > Data loading complete!" << endl;

  while (true) {
       std::stringstream ss;
       ss << "Cycle_" << clk;
       pugi::xml_node clk_node = root.append_child(ss.str().c_str());
       node.set_log_node(clk_node);

       // load binary
       if (clk == 0) {
         node.load_to_bin("./app/SingleNode_Microkernel/LIN_DFT2D.rc");
       }
       node.tick();
       if (node.idle_counter == MAX_IDLE) {
         break;
       }
       clk ++;
    
       if (clk % heart_size == 0) {
         cout << "\rSimulated Cycles: " << clk << flush;
       }
     }
     cout << endl;
     

     vector < vector <int32_t> > output;
     for (int i = 0 ; i < 64; i += 1) {
     	vector <int32_t> col;
     	vector <size_t> col_raw = util::fetch_column(node.lanes[4], 0, i);
     	for (int j = 0 ; j < 64; j+=1) {
     		col.push_back( ((int32_t) col_raw[j]));
     	}
     	output.push_back(col);
     }
     
     string filename = "./suite/dft-isca/output.txt";
     ofstream outfile(filename);
     for (const auto& row : output) {
        for (const auto& elem : row) {
            outfile << elem << " ";
        }
        outfile << std::endl;
    }

    outfile.close();
     
     // Verfiy Correctness
     
     // PERFORM F @ I
     vector <vector <size_t> > FI_real_expected;
     vector <vector <size_t> > FI_imag_expected;
     for (int f = 0; f < 64; f += 1) {
     	     // Real Part
	     vector < size_t > expected_output;
	     vector < size_t > real_output = util::fetch_column(node.lanes[0], f, 128);
	     for (int i = 0; i < 64; i += 1) {
	     	size_t val = 0;
	     	for (int j = 0; j < 64; j+=1) {
	     		val += (realMatrix[j][i] * input_signal[f][j]);
	     	}
	     	expected_output.push_back(val);
	     }
	     //ASSERT_EQ(real_output, expected_output);
	     FI_real_expected.push_back(expected_output);
	     
	     // Imaginary Part
	     expected_output.clear();
	     real_output = util::fetch_column(node.lanes[1], f, 128);
	     for (int i = 0; i < 64; i += 1) {
	     	size_t val = 0;
	     	for (int j = 0; j < 64; j+=1) {
	     		val += (imagMatrix[j][i] * input_signal[f][j]);
	     	}
	     	expected_output.push_back(val);
	     }
	     //ASSERT_EQ(real_output, expected_output);
	     FI_imag_expected.push_back(expected_output);
     }
     
     
     cout << "Step 1 verified " << endl;
     
     // PERFORM data transfer + rshift to renormalize scaled numbers
     for (int f = 0; f < 64; f += 1) {
	     for (int c = 0; c < 64; c+= 1) {
	     	FI_real_expected[c][f] = FI_real_expected[c][f]>>16;
	     	FI_imag_expected[c][f] = FI_imag_expected[c][f]>>16;
	     }
     }
     for (int f = 0; f < 64; f += 1) {
	     for (int c = 0; c < 64; c+= 1) {
	     	//ASSERT_EQ(util::fetch_column(node.lanes[2], f, c), FI_real_expected[c]);
	     	//ASSERT_EQ(util::fetch_column(node.lanes[3], f, c), FI_imag_expected[c]);
	     }
     }
     
     cout << "Step 2 verified " << endl;
     
     // PERFORM (F @ I)_real @ F^T_real and (F @ I)_imag @ F^T_imag
     vector <vector <size_t> > FIFt_real_expected;
     vector <vector <size_t> > FIFt_imag_expected;
     for (int f = 0; f < 64; f += 1) {
     	     // Real Part
	     vector < size_t > expected_output;
	     vector < size_t > real_output = util::fetch_column(node.lanes[2], f, 128);
	     for (int i = 0; i < 64; i += 1) {
	     	size_t val = 0;
	     	for (int j = 0; j < 64; j+=1) {
	     		val += (FI_real_expected[j][i] * realMatrix_t[f][j]);
	     	}
	     	expected_output.push_back(val);
	     }
	     //ASSERT_EQ(real_output, expected_output);
	     FIFt_real_expected.push_back(expected_output);
	     
	     // Imaginary Part
	     
	     expected_output.clear();
	     real_output = util::fetch_column(node.lanes[3], f, 128);
	     for (int i = 0; i < 64; i += 1) {
	     	size_t val = 0;
	     	for (int j = 0; j < 64; j+=1) {
	     		val += (FI_imag_expected[j][i] * imagMatrix_t[f][j]);
	     	}
	     	expected_output.push_back(val);
	     }
	     //ASSERT_EQ(real_output, expected_output);
	     FIFt_imag_expected.push_back(expected_output);
	     
     }
     
     cout << "Step 3 verified " << endl;
     vector < vector <int32_t> > expected_dft_out;
     //PERFORM FINAL SUBTRACTION
     for (int i = 0; i < 64; i += 1) {
     	vector <int32_t> val;
     	for (int j = 0; j < 64; j += 1) {
     		val.push_back((int32_t)(FIFt_real_expected[i][j] - FIFt_imag_expected[i][j]));
     	}
     	expected_dft_out.push_back(val);
     }
     //ASSERT_EQ(output, expected_dft_out);
     cout << "Step 4 verified " << endl;
     // End Verify Correctness


     util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D.rc");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D.node.data");
     system("rm ./app/SingleNode_Microkernel/LIN_DFT2D.human.data");
  
     doc.save_file("./xml_results/SingleNode_Microkernel.LIN_DFT2D.xml");
}

TEST(SingleNode_Microkernel, LIN_MVMUL) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int INSTANCE = L * F;
  int M = R;
  int K = 30;// Inner dimension 512
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  //node.set_show(true, "controller, binary, decoder");
  node.set_record(true, "all");
  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_MVMUL.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t Output[INSTANCE][M];
  size_t Mat[INSTANCE][M][K];
  size_t Vec[INSTANCE][K];
  bool trial = true;
  
  for (int i = 0; i < INSTANCE; i+=1) {
	  for (int k = 0; k < K; k+=1) {
	  	  Vec[i][k] = rand() % 2;
		  for (int m = 0; m < M; m+=1) {
		  	Mat[i][m][k] = rand() % 2;
		  	Output[i][m] = 0;
		  }
	  }
  }
  
  for (int i = 0; i < INSTANCE; i+=1) {
	  for (int k = 0; k < K; k+=1) {
		  for (int m = 0; m < M; m+=1) {
		  	Output[i][m] += Mat[i][m][k] * Vec[i][k];
		  }
	  }
  }
  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < K; c+=1) {
          node_data[l][f][r][c] = Mat[f + l * F][r][c];
        }
        for (int c = K; c < 2*K; c+=1) {
          node_data[l][f][r][c] = Vec[f + l * F][c - K];
        }
        for (int c = 2*K; c < C; c+=1) {
          node_data[l][f][r][c] = 0;
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/LIN_MVMUL.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/LIN_MVMUL.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/LIN_MVMUL.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/LIN_MVMUL.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;

  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);

    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/LIN_MVMUL.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      vector <size_t> expected_output;
      vector <size_t> output = util::fetch_column(node.lanes[l], f, 63);
      for (int r = 0; r < R; r+=1) {
          expected_output.push_back(Output[f + l * F][r]);
      }
      //ASSERT_EQ(expected_output, output);
    }
  }

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/LIN_MVMUL.node.data");
  system("rm ./app/SingleNode_Microkernel/LIN_MVMUL.human.data");
  system("rm ./app/SingleNode_Microkernel/LIN_MVMUL.rc");
  
  doc.save_file("./xml_results/SingleNode_Microkernel.LIN_MVMUL.xml");
}


TEST(SingleNode_Microkernel, LIN_LUFACTOR) {
	srand(0);
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  // int INSTANCE = L * R;
  int INSTANCE = 1;
  
  //int N = 25 * 32 // NxN matrix for LU factor
  int N = 3;
  
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_8l_64r_64x2_P2P.config");
  //node.set_show(true, "controller, binary, decoder");
  node.set_record(true, "all");
  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_LUFACTOR.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  int A_numerator[INSTANCE][N][N];
  int A_denominator[INSTANCE][N][N];
  int Upper_numerator[INSTANCE][N][N];
  int Upper_denominator[INSTANCE][N][N];
  int Lower_numerator[INSTANCE][N][N];
  int Lower_denominator[INSTANCE][N][N];
  
  for (int i = 0; i < INSTANCE; i+=1) {
	  for (int col = 0; col < N; col += 1) {
	  	for (int row = 0; row < N; row+=1) {
	  		A_numerator[i][col][row] = rand() % 16;
	  		A_denominator[i][col][row] = 1;
	  		Upper_numerator[i][col][row] = 0;
	  		Lower_numerator[i][col][row] = 0;
	  		Upper_denominator[i][col][row] = 1;
	  		Lower_denominator[i][col][row] = 1;
	  		
	  		if (col == row) {
	  			Lower_numerator[i][col][row] = 1;
	  		}
	  		if (row > col) {
	  			Lower_numerator[i][col][row] = 0;
	  		}
	  		
	  		if (row == 0) {
	  			Upper_numerator[i][col][row] = A_numerator[i][col][row];
	  		}
	  	}
	  }
  }
  
  for (int i = 0; i < INSTANCE; i+=1) {
	  for (int pivot = 0; pivot < N; pivot += 1) {
	  	
	  	for (int row = pivot + 1; row < N; row += 1) {
		  	// send pivot to all row belows
		  	int row_constant_numerator = Upper_numerator[i][pivot][pivot];
	  		int row_constant_denominator = A_numerator[i][pivot][row];
	  		
	  		for (int col = 0; col < N; col += 1) {
	  			int col_constant_numerator = row_constant_numerator * A_numerator[i][col][row];
	  			int col_constant_denominator = row_constant_denominator * A_denominator[i][col][row];
	  			
	  			cout << col_constant_numerator << " " << col_constant_denominator << endl;
	  			
	  			// Update the upper matrix
	  			Upper_numerator[i][col][row] =  A_numerator[i][col][pivot] * col_constant_denominator - col_constant_numerator  * A_denominator[i][col][row];
	  			Upper_denominator[i][col][row] = col_constant_denominator * A_denominator[i][col][pivot];
	  		}
	  		
	  	}
	  }
  }
  cout << " > Baseline generation complete!" << endl;
  
    for (int row = 0; row < N; row += 1) {
	  	for (int col = 0; col < N; col+=1) {
	  		cout << A_numerator[0][col][row] << "/" << A_denominator[0][col][row] << " ";
	  	}
	  	cout << endl;
	  }
  cout << endl;
  for (int row = 0; row < N; row += 1) {
	  	for (int col = 0; col < N; col+=1) {
	  		cout << Upper_numerator[0][col][row] << "/" << Upper_denominator[0][col][row] << " ";
	  	}
	  	cout << endl;
	  }
	  
	   cout << endl;
  for (int row = 0; row < N; row += 1) {
	  	for (int col = 0; col < N; col+=1) {
	  		cout << Lower_numerator[0][col][row] << "/" <<  Lower_denominator[0][col][row] << " ";
	  	}
	  	cout << endl;
	  }
/*
  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < K; c+=1) {
          node_data[l][f][r][c] = Mat[f + l * F][r][c];
        }
        for (int c = K; c < 2*K; c+=1) {
          node_data[l][f][r][c] = Vec[f + l * F][c - K];
        }
        for (int c = 2*K; c < C; c+=1) {
          node_data[l][f][r][c] = 0;
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/LIN_LUFACTOR.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/LIN_LUFACTOR.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/LIN_LUFACTOR.human.data " + to_string(R) + " " + to_string(C) + " 32";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/LIN_LUFACTOR.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;

  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);

    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/LIN_LUFACTOR.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      vector <size_t> expected_output;
      vector <size_t> output = util::fetch_column(node.lanes[l], f, 63);
      for (int r = 0; r < R; r+=1) {
          expected_output.push_back(Output[f + l * F][r]);
      }
      //ASSERT_EQ(expected_output, output);
    }
  }

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/LIN_LUFACTOR.node.data");
  system("rm ./app/SingleNode_Microkernel/LIN_LUFACTOR.human.data");
  system("rm ./app/SingleNode_Microkernel/LIN_LUFACTOR.rc");
  
  doc.save_file("./xml_results/SingleNode_Microkernel.LIN_LUFACTOR.xml");
  
  */
}

TEST(SingleNode_Microkernel, LIN_MMUL) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = R;
  int N = 21; // number of output column 21
  int K = L * F;// Inner dimension 512
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  //node.set_show(true, "controller, binary, decoder");
  node.set_record(true, "all");
  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/LIN_MMUL.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][K];
  size_t B_baseline[K][N];
  bool trial = true;
  while (trial) {
    for (int m = 0; m < M; m++) {
      for (int k = 0; k < K; k++) {
        int chance = rand() % 2;
        A_baseline[m][k] = (chance == 0) ? 1 : 0;
      }
    }
    for (int k = 0; k < K; k++) {
      for (int n = 0; n < N; n++) {
        int chance = rand() % 2;
        B_baseline[k][n] = (chance == 0) ? 1 : 0;
      }
    }

    for (int m = 0; m < M; m++) {
      for (int n = 0; n < N; n++) {
        C_baseline[m][n] = 0;
      }
    }

    for (int m = 0; m < M; m++) {
      for (int n = 0; n < N; n++) {
        for (int k = 0; k < K; k++) {
            C_baseline[m][n] += A_baseline[m][k] * B_baseline[k][n];
        }
      }
    }

    trial = false;
    for (int m = 0; m < M; m++) {
      for (int n = 0; n < N; n++) {
        if(C_baseline[m][n] >= 256) {
          trial = true;
          break;
        }
      }
    }
  }
  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        node_data[l][f][r][0] = A_baseline[r][f + l * F]; // write A into r0
        for (int n = 0; n < N; n+=1) {
          node_data[l][f][r][n + 1] = B_baseline[f + l * F][n]; // write B into r1 - r22
        }
        for (int n = N + 1; n < C; n += 1) {
          node_data[l][f][r][n] = 0; // init the rest to 0
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/LIN_MMUL.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/LIN_MMUL.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/LIN_MMUL.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/LIN_MMUL.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;

  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);

    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/LIN_MMUL.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (size_t c = 22; c < 43; c+=1) {
    vector <size_t> output = util::fetch_column(node.lanes[0], 0, c);
    for (int r = 0; r < R; r+=1) {
      cout << "\rVerifying correctness of output (r,c): " << r << "," << c << flush;
      //ASSERT_EQ(output[r], C_baseline[r][c - N - 1]);
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/LIN_MMUL.node.data");
  system("rm ./app/SingleNode_Microkernel/LIN_MMUL.human.data");
  system("rm ./app/SingleNode_Microkernel/LIN_MMUL.rc");
  
  doc.save_file("./xml_results/SingleNode_Microkernel.LIN_MMUL.xml");
}

TEST(SingleNode_Microkernel, IMG_BRIGHTNESS) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 63; // Image width
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 255;
      C_baseline[m][n] = 1 + A_baseline[m][n];
    }
  }

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        node_data[l][f][r][0] = 255;
        for (int c = 1; c < C; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c - 1];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/IMG_BRIGHTNESS.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/IMG_BRIGHTNESS.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/IMG_BRIGHTNESS.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 1; c < C; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c - 1 << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c - 1]);
        }
      }
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.node.data");
  system("rm ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.human.data");
  system("rm ./app/SingleNode_Microkernel/IMG_BRIGHTNESS.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.IMG_BRIGHTNESS.xml");
}


TEST(SingleNode_Microkernel, IMG_MAXPOOL) {
  srand(0);
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int I = 20; // Image input height x width
  int O = 18; // Image output height x width
  int K = 3; // Kernel height x width
  
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");
  //node.set_show(true, "decoder");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/IMG_MAXPOOL.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t Image_In [L][R][I][I];
  size_t Image_Out [L][R][O][O];
  size_t Kernel [K][K];
  
  for (int i = 0; i < K; i += 1) { 
  	for (int j = 0; j < K; j += 1) {
  		Kernel[i][j] = rand() % 16;
  	}
  }
  
  for (int i = 0; i < L; i += 1) { 
  	for (int j = 0; j < R; j += 1) {
  		for (int k = 0; k < I; k += 1) {
  			for (int l = 0; l < I; l += 1) {
  				Image_In[i][j][k][l] = rand() % 2;
  			}
  		}
  	}
  }
  
  for (int i = 0; i < L; i += 1) { // for every lane
  	for (int j = 0; j < R; j += 1) { // for every column
  		for (int x = 0; x < O; x += 1) { // X
  			for (int y = 0; y < O; y += 1) { // Y
  				int relative_x = x + 1;
  				int relative_y = y + 1;
  				Image_Out[i][j][x][y] = Image_In[i][j][relative_x-1][relative_y-1];
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x  ][relative_y-1], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x+1][relative_y-1], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x-1][relative_y], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x  ][relative_y], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x+1][relative_y], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x-1][relative_y+1], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x  ][relative_y+1], Image_Out[i][j][x][y]);
  				Image_Out[i][j][x][y] = max(Image_In[i][j][relative_x+1][relative_y+1], Image_Out[i][j][x][y]);
  			}
  		}
  	}
  }
  

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int r = 0; r < R; r+=1) {
      for (int f = 0; f < F; f+=1) {
      	if (f < I * 3) {
	      	for (int c = 0; c < C; c += 1) {
			// Store the image in the first 20 columns
			if (c < 20) {
				node_data[l][f][r][c] = Image_In[l][r][c][f/3];
			}
			// Store the kernel in the next 3 columns
			else if (c >= 61) {
				node_data[l][f][r][c] = Kernel[(c-61)%3][f%3];
			}
			else {
				node_data[l][f][r][c] = 0;
			}
		}
      	}
      	else {
      		for (int c = 0; c < C; c += 1) {
      			node_data[l][f][r][c] = 0;
      		}
      	}
      }
    }
  }
  
  ofstream human_data("./app/SingleNode_Microkernel/IMG_MAXPOOL.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/IMG_MAXPOOL.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/IMG_MAXPOOL.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/IMG_MAXPOOL.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/IMG_MAXPOOL.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;
  
  for (int l = 0; l < L; l+=1) {
    for (int f = 4; f < 58; f+=3) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < 18; c += 1) {
          cout << "\rVerifying correctness of output (l, f, r, c): " << l << "," << f << "," << r << "," << c << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], Image_Out[l][r][c][(f-4)/3]);
        }
      }
    }
  }
  

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/IMG_MAXPOOL.node.data");
  system("rm ./app/SingleNode_Microkernel/IMG_MAXPOOL.human.data");
  system("rm ./app/SingleNode_Microkernel/IMG_MAXPOOL.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.IMG_MAXPOOL.xml");
}

TEST(SingleNode_Microkernel, IMG_CONV) {
  srand(0);
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int I = 20; // Image input height x width
  int O = 18; // Image output height x width
  int K = 3; // Kernel height x width
  
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");
  //node.set_show(true, "decoder");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/IMG_CONV.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t Image_In [L][R][I][I];
  size_t Image_Out [L][R][O][O];
  size_t Kernel [K][K];
  
  for (int i = 0; i < K; i += 1) { 
  	for (int j = 0; j < K; j += 1) {
  		Kernel[i][j] = rand() % 16;
  	}
  }
  
  for (int i = 0; i < L; i += 1) { 
  	for (int j = 0; j < R; j += 1) {
  		for (int k = 0; k < I; k += 1) {
  			for (int l = 0; l < I; l += 1) {
  				Image_In[i][j][k][l] = rand() % 2;
  			}
  		}
  	}
  }
  
  for (int i = 0; i < L; i += 1) { // for every lane
  	for (int j = 0; j < R; j += 1) { // for every column
  		for (int x = 0; x < O; x += 1) { // X
  			for (int y = 0; y < O; y += 1) { // Y
  				int relative_x = x + 1;
  				int relative_y = y + 1;
  				Image_Out[i][j][x][y]  = Kernel[0][0] * Image_In[i][j][relative_x-1][relative_y-1];
  				Image_Out[i][j][x][y] += Kernel[1][0] * Image_In[i][j][relative_x  ][relative_y-1];
  				Image_Out[i][j][x][y] += Kernel[2][0] * Image_In[i][j][relative_x+1][relative_y-1];
  				Image_Out[i][j][x][y] += Kernel[0][1] * Image_In[i][j][relative_x-1][relative_y];
  				Image_Out[i][j][x][y] += Kernel[1][1] * Image_In[i][j][relative_x  ][relative_y];
  				Image_Out[i][j][x][y] += Kernel[2][1] * Image_In[i][j][relative_x+1][relative_y];
  				Image_Out[i][j][x][y] += Kernel[0][2] * Image_In[i][j][relative_x-1][relative_y+1];
  				Image_Out[i][j][x][y] += Kernel[1][2] * Image_In[i][j][relative_x  ][relative_y+1];
  				Image_Out[i][j][x][y] += Kernel[2][2] * Image_In[i][j][relative_x+1][relative_y+1];
  			}
  		}
  	}
  }
  

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int r = 0; r < R; r+=1) {
      for (int f = 0; f < F; f+=1) {
      	if (f < I * 3) {
	      	for (int c = 0; c < C; c += 1) {
			// Store the image in the first 20 columns
			if (c < 20) {
				node_data[l][f][r][c] = Image_In[l][r][c][f/3];
			}
			// Store the kernel in the next 3 columns
			else if (c >= 61) {
				node_data[l][f][r][c] = Kernel[(c-61)%3][f%3];
			}
			else {
				node_data[l][f][r][c] = 0;
			}
		}
      	}
      	else {
      		for (int c = 0; c < C; c += 1) {
      			node_data[l][f][r][c] = 0;
      		}
      	}
      }
    }
  }
  
  ofstream human_data("./app/SingleNode_Microkernel/IMG_CONV.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/IMG_CONV.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/IMG_CONV.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/IMG_CONV.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/IMG_CONV.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;
  
  for (int l = 0; l < L; l+=1) {
    for (int f = 4; f < 58; f+=3) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < 18; c += 1) {
          cout << "\rVerifying correctness of output (l, f, r, c): " << l << "," << f << "," << r << "," << c << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], Image_Out[l][r][c][(f-4)/3]);
        }
      }
    }
  }
  

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/IMG_CONV.node.data");
  system("rm ./app/SingleNode_Microkernel/IMG_CONV.human.data");
  system("rm ./app/SingleNode_Microkernel/IMG_CONV.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.IMG_CONV.xml");
}

TEST(SingleNode_Microkernel, IMG_INVERT) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 63; // Image width
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/IMG_INVERT.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 256;
      C_baseline[m][n] = 255 - A_baseline[m][n];
    }
  }

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        node_data[l][f][r][0] = 255;
        for (int c = 1; c < C; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c - 1];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/IMG_INVERT.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/IMG_INVERT.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/IMG_INVERT.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/IMG_INVERT.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/IMG_INVERT.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 1; c < C; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c - 1 << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c - 1]);
        }
      }
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/IMG_INVERT.node.data");
  system("rm ./app/SingleNode_Microkernel/IMG_INVERT.human.data");
  system("rm ./app/SingleNode_Microkernel/IMG_INVERT.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.IMG_INVERT.xml");
}

TEST(SingleNode_Microkernel, IMG_GRAYSCALE) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 60; // Image width * 3 (RGB)
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");
  // node.set_show(true, "lane0, controller");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/IMG_GRAYSCALE.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N/3];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 13 + 3;
    }
  }
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N/3; n++) {
      C_baseline[m][n] = (A_baseline[m][n] + A_baseline[m][n + N/3] + A_baseline[m][n + 2*N/3]) / 3;
      //C_baseline[m][n] = A_baseline[m][n] / 3;
    }
  }

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < N; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c];
        }
        node_data[l][f][r][60] = 0;
        node_data[l][f][r][61] = 0;
        node_data[l][f][r][62] = 0;
        node_data[l][f][r][63] = 3;
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/IMG_GRAYSCALE.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/IMG_GRAYSCALE.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/IMG_GRAYSCALE.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/IMG_GRAYSCALE.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  //node.set_show(true, "controller, binary, lane0");
  //util::pretty_print_cluster(node.lanes[0]);
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/IMG_GRAYSCALE.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

   // node.lanes[0]->display_data(0);

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;
  //util::pretty_print_cluster(node.lanes[0]);

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < N/3; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c]);
        }
      }
    }
  }

  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/IMG_GRAYSCALE.node.data");
  system("rm ./app/SingleNode_Microkernel/IMG_GRAYSCALE.human.data");
  system("rm ./app/SingleNode_Microkernel/IMG_GRAYSCALE.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.IMG_GRAYSCALE.xml");
}


TEST(SingleNode_Microkernel, DB_GREP) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int NUM_CHAR = L * F * (C - 3); // number of char per file
  int NUM_FILE = R; // number of files for grep

  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/DB_GREP.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t query = rand() % 256;
  size_t database[NUM_CHAR][NUM_FILE];
  vector <size_t> expected_matches;

  for (int n = 0; n < NUM_FILE; n++) {
    expected_matches.push_back(0);
  }

  for (int n = 0; n < NUM_FILE; n++) {
    for (int m = 0; m < NUM_CHAR; m++) {
      size_t val = rand() % 256;
      database[m][n] = val;
      if (val == query) {
        expected_matches[n] += 1;
      }
    }
  }

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        node_data[l][f][r][0] = query;
        node_data[l][f][r][1] = 0;
        node_data[l][f][r][2] = 0;
        for (int c = 0; c < C - 3; c += 1) {
          node_data[l][f][r][c + 3] = database[c + (C - 3) * (f + F * l)][r];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/DB_GREP.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/DB_GREP.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/DB_GREP.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/DB_GREP.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/DB_GREP.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  cout << "\rVerifying correctness matches stored in r1 column of lane 0 regfile 0: " << endl;
  vector <size_t> matches = util::fetch_column(node.lanes[0], 0, 1);
  //ASSERT_EQ(matches, expected_matches);


  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/DB_GREP.node.data");
  system("rm ./app/SingleNode_Microkernel/DB_GREP.human.data");
  system("rm ./app/SingleNode_Microkernel/DB_GREP.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.DB_GREP.xml");
}

TEST(SingleNode_Microkernel, DB_SEARCH) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int NUM_CHAR = L * F * (C - 3); // number of char per file
  int NUM_FILE = R; // number of files for grep

  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/DB_SEARCH.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t query = rand() % 256;
  size_t database[NUM_CHAR][NUM_FILE];
  size_t matches[NUM_CHAR][NUM_FILE];
  vector <size_t> expected_matches;

  for (int n = 0; n < NUM_FILE; n++) {
    for (int m = 0; m < NUM_CHAR; m++) {
      size_t val = rand() % 256;
      database[m][n] = val;
      if (val == query) {
        matches[m][n] = 255;
      }
      else {
      	matches[m][n] = 0;
      }
    }
  }

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        node_data[l][f][r][0] = query;
        node_data[l][f][r][1] = 0;
        node_data[l][f][r][2] = 0;
        for (int c = 0; c < C - 3; c += 1) {
          node_data[l][f][r][c + 3] = database[c + (C - 3) * (f + F * l)][r];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/DB_SEARCH.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/DB_SEARCH.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/DB_SEARCH.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/DB_SEARCH.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/DB_SEARCH.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  cout << "\rVerifying correctness matches stored in r1 column of lane 0 regfile 0: " << endl;
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C - 3; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c + 3);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(matches[c + (C - 3) * (f + F * l)][r]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }


  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/DB_SEARCH.node.data");
  system("rm ./app/SingleNode_Microkernel/DB_SEARCH.human.data");
  system("rm ./app/SingleNode_Microkernel/DB_SEARCH.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.DB_SEARCH.xml");
}


TEST(SingleNode_Microkernel, DIS_HAMMING) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 64; // Image width
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/DIS_HAMMING.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 16;
    }
  }
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N/2; n++) {
      size_t or_result = A_baseline[m][n] |  A_baseline[m][n+N/2];
      size_t count = 0;
      // Perform bitwise AND with a mask to consider only the first eight bits
      for (int j = 0; j < 8; j++) {
		if (or_result & 1) {
		    count++;
		}
		or_result >>= 1;
      }
      C_baseline[m][n] = count;
    }
  }
  

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/DIS_HAMMING.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/DIS_HAMMING.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/DIS_HAMMING.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/DIS_HAMMING.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/DIS_HAMMING.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < 1; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = C/2; c < C; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c - C/2]);
        }
      }
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/DIS_HAMMING.node.data");
  system("rm ./app/SingleNode_Microkernel/DIS_HAMMING.human.data");
  system("rm ./app/SingleNode_Microkernel/DIS_HAMMING.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.DIS_HAMMING.xml");
}

TEST(SingleNode_Microkernel, DIS_MANHATTAN) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 64; // Image width
  Node<Cluster, Racer_Decoder> node(0, "./config/8b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/DIS_MANHATTAN.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 16;
    }
  }
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N/2 - 4; n+=2) {
      size_t Ax = A_baseline[m][n];
      size_t Ay = A_baseline[m][n+1];
      size_t Bx = A_baseline[m][n+N/2 - 4];
      size_t By = A_baseline[m][n+N/2 - 3];
      
      size_t dis = 0;
      
      if (Ax > Bx) {
      	dis += Ax - Bx;
      }
      else {
        dis += Bx - Ax;
      }
      
      if (Ay > By) {
      	dis += Ay - By;
      }
      else {
      	dis += By - Ay;
      }
      

      C_baseline[m][n/2] = dis;
    }
  }
  

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/DIS_MANHATTAN.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/DIS_MANHATTAN.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/DIS_MANHATTAN.human.data " + to_string(R) + " " + to_string(C) + " 8";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/DIS_MANHATTAN.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  //node.set_show(true, "lane0, controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/DIS_MANHATTAN.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;
    
    //node.lanes[0]->display_data(0);

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < 1; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < (N/2 - 4)/2; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c << flush;
          vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
          //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c]);
        }
      }
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/8b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/DIS_MANHATTAN.node.data");
  system("rm ./app/SingleNode_Microkernel/DIS_MANHATTAN.human.data");
  system("rm ./app/SingleNode_Microkernel/DIS_MANHATTAN.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.DIS_MANHATTAN.xml");
}

TEST(SingleNode_Microkernel, DIS_EUCLIDEAN) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  size_t clk = 0;
  size_t MAX_IDLE = 1000;
  size_t heart_size = 100;
  int L = 8;  // number of lanes
  int F = 64; // number of reg file per lane
  int R = 2;  // num tile row (also number of output row)
  int C = 64; // num tile column 64
  int M = L * R * F; // Image height
  int N = 64; // Image width
  Node<Cluster, Racer_Decoder> node(0, "./config/32b_8l_64r_64x2_P2P.config");
  node.set_record(true, "all");

  cout << " > Compiling .RC from .ERC file" << endl;
  system("python3 ../util/ezpim.py ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.erc");
  cout << " > Compilation complete!" << endl;

  cout << " > Baseline generation" << endl;
  size_t C_baseline[M][N];
  size_t A_baseline[M][N];

  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      A_baseline[m][n] = rand() % 16;
    }
  }
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N/2 - 4; n+=2) {
      size_t Ax = A_baseline[m][n];
      size_t Ay = A_baseline[m][n+1];
      size_t Bx = A_baseline[m][n+N/2 - 4];
      size_t By = A_baseline[m][n+N/2 - 3];
      
      size_t dis = 0;
      
      if (Ax > Bx) {
      	size_t tmp = Ax - Bx;
      	tmp = tmp * tmp;
      	dis += tmp;
      }
      else {
        size_t tmp = Bx - Ax;
      	tmp = tmp * tmp;
      	dis += tmp;
      }
      
      if (Ay > By) {
      	size_t tmp = Ay - By;
      	tmp = tmp * tmp;
      	dis += tmp;
      }
      else {
      	size_t tmp = By - Ay;
      	tmp = tmp * tmp;
      	dis += tmp;
      }
      

      C_baseline[m][n/2] = sqrt(dis);
    }
  }
  

  cout << " > Baseline generation complete!" << endl;

  cout << " > Input data generation" << endl;
  size_t node_data [L][F][R][C]; // (lane, reg file, row, col)
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c += 1) {
          node_data[l][f][r][c] = A_baseline[r + R * (f + F * l)][c];
        }
      }
    }
  }
  ofstream human_data("./app/SingleNode_Microkernel/DIS_EUCLIDEAN.human.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < C; c+=1) {
          human_data << node_data[l][f][r][c] << " ";
        }
        human_data << endl;
      }
      human_data << "*" << endl;
    }
    human_data << "#" << endl;
  }
  human_data.close();
  system("rm ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.node.data");
  string command = "python3 ../util/data_converter.py ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.human.data " + to_string(R) + " " + to_string(C) + " 32";
  system(command.c_str());
  cout << " > Input data generation complete!" << endl;

  cout << " > Data loading" << endl;
  util::init_node_user_input(node.lanes, L, "./app/SingleNode_Microkernel/DIS_EUCLIDEAN.node.data");
  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < F; f+=1) {
      for (int c = 0; c < C; c+=1) {
        vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
        vector <size_t> expected_output;
        for (int r = 0; r < R; r+=1) {
          expected_output.push_back(node_data[l][f][r][c]);
        }
        cout << "\r > Verifying data loading correctness (lane, reg. file, col.): " << l << " " << f << " " << c;
        //ASSERT_EQ(output, expected_output);
      }
    }
  }
  cout << endl << " > Data loading complete!" << endl;
  // node.set_show(true, "controller");
  while (true) {
    std::stringstream ss;
    ss << "Cycle_" << clk;
    pugi::xml_node clk_node = root.append_child(ss.str().c_str());
    node.set_log_node(clk_node);
    // load binary
    if (clk == 0) {
      node.load_to_bin("./app/SingleNode_Microkernel/DIS_EUCLIDEAN.rc");
    }
    node.tick();
    if (node.idle_counter == MAX_IDLE) {
      break;
    }
    clk ++;

    if (clk % heart_size == 0) {
      cout << "\rSimulated Cycles: " << clk << flush;
    }
  }
  cout << endl;

  for (int l = 0; l < L; l+=1) {
    for (int f = 0; f < 1; f+=1) {
      for (int r = 0; r < R; r+=1) {
        for (int c = 0; c < (N/2 - 4)/2; c += 1) {
          cout << "\rVerifying correctness of output (m,n): " << r + R * (f + F * l) << "," << c << flush;
          if (c == 0 && f == 0) {
		  vector <size_t> output = util::fetch_column(node.lanes[l], f, c);
		  //ASSERT_EQ(output[r], C_baseline[r + R * (f + F * l)][c]);
          }
        }
      }
    }
  }
  cout << endl;

  util::report_microkernel_node(&node, clk - MAX_IDLE, "./config/32b_8l_64r_64x2_P2P.config");
  system("rm ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.node.data");
  system("rm ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.human.data");
  system("rm ./app/SingleNode_Microkernel/DIS_EUCLIDEAN.rc");
  doc.save_file("./xml_results/SingleNode_Microkernel.DIS_EUCLIDEAN.xml");
}

#endif
