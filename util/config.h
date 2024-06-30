#ifndef CONFIG_H
#define CONFIG_H

#include "../src/middle-racer/cluster.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

namespace util {
  inline string general_config_parser(string field_name, string file_name) {
    ifstream file(file_name);
    string value;
    string lastInstance;
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.find(field_name) != string::npos) {
                lastInstance = line;
            }
        }
        if (!lastInstance.empty()) {
          size_t pos = lastInstance.find_first_of("=");
          // Remove any whitespace characters after the delimiter
          string value = lastInstance.substr(pos + 1);
          value.erase(0, value.find_first_not_of(" \t"));
          file.close();
          return value;
        }
        else {
            cout << "ERROR: " + field_name + "not found!" << endl;
            file.close();
            exit(-1);
        }
    }
    else {
        cout << "ERROR: Unable to open file!" << endl;
        file.close();
        exit(-1);
    }
    file.close();
    return NULL;
  };
};
#endif
