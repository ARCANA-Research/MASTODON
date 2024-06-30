#ifndef RERAM_FELIX_H
#define RERAM_FELIX_H

#include <vector>
#include <deque>
#include <map>
#include <utility>
#include <string>

using namespace std;

class ReRAM_FELIX
{
public:
  ReRAM_FELIX(){};
  map <string, deque<string> > get_recipe() {
    map <string, deque<string> > table;
    // Add recipes here
    return table;
  };
};

#endif
