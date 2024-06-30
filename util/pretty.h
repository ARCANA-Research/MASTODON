#ifndef PRETTY_H
#define PRETTY_H

#include "../src/bottom/column.h"
#include "../src/middle-mimdram/mat.h"
#include "../src/middle-racer/cluster.h"
#include "../src/top/node.h"
#include <string>
#include <vector>
#include <map>
#include <bitset>

namespace util {
  inline void pretty_print_cluster(Cluster * cluster) {
    size_t num_pipeline = cluster->num_pipeline;
    size_t num_col = cluster->num_col;
    size_t num_row = cluster->num_row;
    size_t num_bg = cluster->granularity / 8;

    for (size_t p = 0; p < num_pipeline; p += 1) {
      cout << "Register File " << p << endl;
      for (size_t r = 0; r < num_row; r += 1) {
        for (size_t c = 0; c < num_col; c += 1) {
          size_t value = 0;
          for (int b = num_bg - 1; b >= 0; b -= 1) {
            size_t portion = cluster->get_byte(p, b, r, c);
            portion = portion << (8 * b);
            value += portion;
          }
          cout << value << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
  };

  inline vector <size_t> fetch_column(Cluster * cluster, size_t regfile_idx, size_t column_idx) {
    size_t num_row = cluster->num_row;
    size_t num_bg = cluster->granularity / 8;
    vector <size_t> column;
    for (size_t r = 0; r < num_row; r += 1) {
      size_t value = 0;
      for (int b = num_bg - 1; b >= 0; b -= 1) {
        size_t portion = cluster->get_byte(regfile_idx, b, r, column_idx);
        portion = portion << (8 * b);
        value += portion;
      }
      column.push_back(value);
    }
    return column;
  };

  inline vector <size_t> fetch_column(Mat * mat, size_t column_idx) {
    size_t num_row = mat->num_row;
    size_t elemsize = mat->granularity;
    vector <size_t> column;
    vector <string> raw_data;

    for (size_t e = 0; e < elemsize; e+=1) {
      raw_data.push_back(mat->fetch_regular_register(0, column_idx, e)->raw);
    }

    for (auto r = 0; r < num_row; r+= 1) {
      string raw_binary = "";
      for (size_t e = 0; e < elemsize; e+=1) {
        raw_binary += raw_data[e][r];
      }
      std::bitset<sizeof(size_t) * 8> number(raw_binary);
      column.push_back(number.to_ulong());
    }
    return column;
  }

  inline void report_peak_node(Node<Cluster, Racer_Decoder> * node, size_t clk, string config, size_t num_repeated_op) {
    map <string, size_t> tmp = node->report_primitive_op_count();
    size_t total_primitive = (tmp["FA"] + tmp["NAND"] + tmp["NOR"] + tmp["AND"] + tmp["OR"] + tmp["XOR"] + tmp["NOT"] + tmp["COPY"])/num_repeated_op;
    size_t granularity = stof(util::general_config_parser("granularity", config));
    float energy_per_primitive = stof(util::general_config_parser("pJ_per_vec_primitive", config));
    float cycle_per_sec = stof(util::general_config_parser("MHz_frequency", config));
    float area = stof(util::general_config_parser("cm2_total_area", config));
    float vector_length = stof(util::general_config_parser("vector_length", config));
    float TDP = stof(util::general_config_parser("watt_thermal_limit", config));

    cout << "============================================================" << endl;
    cout << "\033[34mvvvvvvvvvvvv Architectural Overview vvvvvvvvvvvv\033[0m" << endl;
    cout << "Simulated Cycles: " << clk << endl;
    cout << "Vector Throughput per. Pipeline (cycles/vector/pipeline): " << clk / num_repeated_op << endl;
    cout << "Peak Instruction Throughput w. RACER-4096 MICRO'21 setup (GOp./s): " << (64 / granularity * 4096 * 64) / (clk / num_repeated_op) * (333 * 1000000) / 1000000000 << endl;
    cout << "Energy Efficiency w. RACER-4096 MICRO'21 setup (GOp./s/W): " << ((64 / granularity * 4096 * 64) / (clk / num_repeated_op) * (333 * 1000000) / 1000000000) / ((64 / granularity * 4096) * (total_primitive * energy_per_primitive / vector_length) / (clk / num_repeated_op) * cycle_per_sec / 1000000) << endl;
    cout << "Peak Instruction Throughput w. " << TDP << " Watt TDP (GOp./s): " << TDP*(((64 / granularity * 4096 * 64) / (clk / num_repeated_op) * (333 * 1000000) / 1000000000) / ((64 / granularity * 4096) * (total_primitive * energy_per_primitive / vector_length) / (clk / num_repeated_op) * cycle_per_sec / 1000000)) << endl;
    cout << "Uop Queue Activity Factor: " << node->report_activity_factor(clk) << endl;
    cout << "Primitive Count: " << endl;
    cout << "      |- FA  : " << tmp["FA"] / num_repeated_op << endl;
    cout << "      |- NAND: " << tmp["NAND"] / num_repeated_op << endl;
    cout << "      |- NOR : " << tmp["NOR"] / num_repeated_op << endl;
    cout << "      |- AND : " << tmp["AND"] / num_repeated_op << endl;
    cout << "      |- OR  : " << tmp["OR"] / num_repeated_op << endl;
    cout << "      |- XOR : " << tmp["XOR"] / num_repeated_op << endl;
    cout << "      |- NOT : " << tmp["NOT"] / num_repeated_op << endl;
    cout << "      |- COPY: " << tmp["COPY"] / num_repeated_op << endl;

    cout << "\033[34mvvvvvvvvvvvv Process-Specific Numbers vvvvvvvvvvvv\033[0m" << endl;
    cout << "Process Name: " << util::general_config_parser("process_name", config) << endl;
    cout << "Instruction Vector Length: " << vector_length << endl;
    cout << "Clock Frequency (MHz): " << cycle_per_sec << endl;
    cout << "Total Chip Area (cm^2): " << area << endl;
    cout << "Total Power (mW): " << (total_primitive * energy_per_primitive) / (clk / num_repeated_op) * cycle_per_sec / 1000 << endl;
    cout << "Thermal Density (W/cm^2): " << (total_primitive * energy_per_primitive) / (clk / num_repeated_op) * cycle_per_sec / 1000000 /area << endl;
    cout << "Energy per. Vector Instruction (pJ/op.): " << total_primitive * energy_per_primitive << endl;
    cout << "Peak Instruction Throughput per. Area (Gop./second/cm^2): " << vector_length / (clk / num_repeated_op) * (cycle_per_sec * 1000000) / 1000000000 / area << endl;
    cout << "============================================================" << endl;
};

inline void report_microkernel_node(Node<Cluster, Racer_Decoder> * node, size_t clk, string config) {
  cout << "============================================================" << endl;
  cout << "\033[34mvvvvvvvvvvvv Architectural Overview vvvvvvvvvvvv\033[0m" << endl;
  cout << "Simulated Cycles: " << clk << endl;
  cout << "Uop Queue Activity Factor: " << node->report_activity_factor(clk) << endl;
  map <string, size_t> tmp = node->report_primitive_op_count();
  cout << "Primitive Count (in millions): " << endl;
  cout << "      |- FA  : " << (float) tmp["FA"] / 1000000 << endl;
  cout << "      |- NAND: " << (float) tmp["NAND"] / 1000000 << endl;
  cout << "      |- NOR : " << (float) tmp["NOR"] / 1000000 << endl;
  cout << "      |- AND : " << (float) tmp["AND"] / 1000000 << endl;
  cout << "      |- OR  : " << (float) tmp["OR"] / 1000000 << endl;
  cout << "      |- XOR : " << (float) tmp["XOR"] / 1000000 << endl;
  cout << "      |- NOT : " << (float) tmp["NOT"] / 1000000 << endl;
  cout << "      |- COPY: " << (float) tmp["COPY"] / 1000000 << endl;
  cout << "\033[34mvvvvvvvvvvvv Process-Specific Numbers vvvvvvvvvvvv\033[0m" << endl;
  size_t total_primitive = (tmp["NAND"] + tmp["NOR"] + tmp["AND"] + tmp["OR"] + tmp["XOR"] + tmp["NOT"] + tmp["COPY"]);
  float energy_per_primitive = stof(util::general_config_parser("pJ_per_vec_primitive", config));
  float cycle_per_sec = stof(util::general_config_parser("MHz_frequency", config));
  float area = stof(util::general_config_parser("cm2_total_area", config));
  cout << "Process Name: " << util::general_config_parser("process_name", config) << endl;
  cout << "Clock Frequency (MHz): " << cycle_per_sec << endl;
  cout << "Total Chip Area (cm^2): " << area << endl;
  cout << "Total Power (mW): " << (total_primitive * energy_per_primitive) / (clk) * cycle_per_sec / 1000 << endl;
  cout << "Thermal Density (W/cm^2): " << (total_primitive * energy_per_primitive) / (clk) * cycle_per_sec / 1000000 /area << endl;
  cout << "Energy Consumption (J): " << total_primitive * energy_per_primitive / 1000000000000 << endl;
  cout << "============================================================" << endl;
};
}

#endif
