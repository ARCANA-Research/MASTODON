#ifndef INIT_H
#define INIT_H

//INTEGRATION NOTICE: Add your lane definition here
#include "../src/middle-racer/cluster.h"
#include "../src/middle-mimdram/mat.h"
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

namespace util {
  inline string generate_binary_string(size_t N, size_t starting_bit) {
    string ret = "";
    for (size_t i = 0; i < N; i++) {
        if (i >= starting_bit) {
          ret += "0";
        }
        else {
          int x = ((int)rand() % 2);
          ret += to_string(x);
        }
    }  
    return ret;
  };

  inline void init_lane_random(Cluster * cluster, size_t starting_bit){
    for (size_t p = 0; p < cluster->num_pipeline; p+=1) {
      vector <vector <string> > data;
      for (size_t c = 0; c < cluster->num_col; c+=1) {
        vector <string> column;
        for (size_t r = 0; r < cluster->num_row; r+=1) {
          column.push_back(generate_binary_string(cluster->granularity, starting_bit));
        }
        data.push_back(column);
      }
      cluster->write_data(p, data);
    }
  };

  inline void init_lane_random(Mat * mat, size_t starting_bit) {
    ;
  }

  // inline void init_tile_random(Tile * tile){
  //   vector <string> data;
  //   for (size_t c = 0; c < tile->num_col; c+=1) {
  //     data.push_back(generate_binary_string(tile->num_row));
  //   }
  //   tile->write_data(data);
  // };

  inline string read_string_from_file(string file_addr) {
    ifstream file(file_addr);
    if (file.is_open()) {
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        content.erase(remove(content.begin(), content.end(), '\n'), content.end());
        // std::cout << content << std::endl;
        file.close();
        return content;
    }
    else {
        cout << "Failed to open file." << endl;
    }
    return NULL;
  };

  inline void init_cluster_user_input(Cluster * cluster, string file_addr) {
    string substring = read_string_from_file(file_addr);
    for (size_t p = 0; p < cluster->num_pipeline; p+=1) {
      vector <vector <string> > data;
      for (size_t c = 0; c < cluster->num_col; c+=1) {
        vector <string> column;
        for (size_t r = 0; r < cluster->num_row; r+=1) {
          string number = substring.substr(0, cluster->granularity);
          reverse(number.begin(), number.end());
          column.push_back(number);
          substring = substring.substr(cluster->granularity);
        }
        data.push_back(column);
      }
      cluster->write_data(p, data);
    }
  };


  // This function is retired, do not use, use the read_data_from_file (below) instead
  inline void init_node_user_input(vector <Cluster *> lanes, size_t num_lane, string file_addr) {
    string substring = read_string_from_file(file_addr);
    for (size_t l = 0; l < num_lane; l+=1) {
      for (size_t p = 0; p < lanes[0]->num_pipeline; p+=1) {
        vector <vector <string> > data;
        for (size_t c = 0; c < lanes[0]->num_col; c+=1) {
          vector <string> column;
          for (size_t r = 0; r < lanes[0]->num_row; r+=1) {
            string number = substring.substr(0, lanes[0]->granularity);
            reverse(number.begin(), number.end());
            column.push_back(number);
            substring = substring.substr(lanes[0]->granularity);
          }
          data.push_back(column);
        }
        lanes[l]->write_data(p, data); // data format Col, Row, Tile/Granularity)
      }
    }
  };

inline void write_data_to_file(vector <vector <vector <vector <size_t> > > > data_in, string file_addr, int num_lane, int num_regfile, int num_col, int num_row) {
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");
  pugi::xml_node node = root.append_child("in");
  for (int l = 0; l < num_lane; l+=1) {
    pugi::xml_node lane_node = node.append_child(("L" + to_string(l)).c_str());
    for (int f = 0; f < num_regfile; f+=1) {
      pugi::xml_node regfile_node = lane_node.append_child(("RF" + to_string(f)).c_str());
      for (int c = 0; c < num_col; c+=1) {
        string data = "";
        for (int r = 0; r < num_row; r+=1) {
          data += to_string(data_in[l][f][c][r]);
          data += " ";
        }
        regfile_node.append_child(("C" + to_string(c)).c_str()).text() = data.c_str();
      }
    }
  }
  doc.save_file(file_addr.c_str());
};

inline void read_data_from_file(vector <Cluster *> lanes, string file_addr){
    size_t num_row = lanes[0]->num_row;
    size_t granularity = lanes[0]->granularity;

    ifstream file(file_addr);
    if(!file){
      cerr<<"error! can't find file at address: "<<file_addr<<endl;
      exit(0);
    }
    string line;
    size_t lane_idx, rf_idx;//, col_idx;
    vector <vector <string> > data;
    vector <string> column;
    while (getline(file, line)){
      size_t found = line.find('<');
      // delete the front whitespacess
      string content = line.substr(found); 
      found = content.find('>');
      if (content[1] == 'L'){
          // Lane
        lane_idx = stoi(content.substr(2,found-2));
      }else if(content[1] == 'R'){
        // Register file
        rf_idx = stoi(content.substr(3,found-3));
      }else if(content[1] == 'C'){
        // column data
        //col_idx = stoi(content.substr(2,found-2));
        istringstream iss(content.substr(found+1));
        for(size_t i = 0; i < num_row; i++){
          size_t val;
          iss >> val;
          string binary;
          if (granularity == 8) {
            binary = bitset<8>(val).to_string(); //to binary
          }
          else if (granularity == 16) {
            binary = bitset<16>(val).to_string(); //to binary
          }
          else if (granularity == 32) {
            binary = bitset<32>(val).to_string(); //to binary
          }
          else if (granularity == 64) {
            binary = bitset<64>(val).to_string(); //to binary
          }
          reverse(binary.begin(),binary.end());
          column.push_back(binary);
        }
        data.push_back(column);
        column.clear();
          // rest casess all start from '/'
      }else if (content[2] == 'R'){
      // end of one Register file
        lanes[lane_idx]->write_data(rf_idx, data);  
        data.clear();
      }else if(content[2] == 'i'){
        // end of input data
        break;
        file.close();
      }
    }
}
};

#endif
