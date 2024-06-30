#ifndef RECIPE_TABLE
#define RECIPE_TABLE
#include <vector>
#include <deque>
#include <map>
#include <utility>
#include "ReRAM_magic.h"
#include "ReRAM_felix.h"
#include "ReRAM_oscar.h"
#include "ReRAM_ideal.h"
#include "DRAM_ambit_rowclone.h"
#include "MultiTech_NMP.h"

enum PUMtech {MAGIC, // NOR (ReRAM)
              FELIX, // NAND, NOR, OR, 2-cycle XOR (ReRAM)
              OSCAR, // NOR, OR (ReRAM)
              NMP,  // FA + Ideal (1-cycle XOR)
              IDEAL, // NOR, NAND, OR, AND, 2-cycle XOR (ideal ReRAM)
              AMBIT_ROWCLONE
            };

using namespace std;
class RecipeTable
{
public:
  PUMtech type;

  RecipeTable(PUMtech type_){
    this->type = type_;
    this->setup();
  };

  deque<string> get_recipe(string command) {
    return this->table[this->type][command];
  };

private:
  map <PUMtech, map <string, deque<string> > > table;
  void setup() {
    if (this->type == MAGIC) {
      ReRAM_MAGIC magic;
      this->table.insert({MAGIC, magic.get_recipe()});
    }
    else if (this->type == FELIX) {
      ReRAM_FELIX felix;
      this->table.insert({FELIX, felix.get_recipe()});
    }
    else if (this->type == OSCAR) {
      ReRAM_OSCAR oscar;
      this->table.insert({OSCAR, oscar.get_recipe()});
    }
    else if (this->type == IDEAL) {
      ReRAM_IDEAL ideal;
      this->table.insert({IDEAL, ideal.get_recipe()});
    }
    else if (this->type == NMP) {
      MultiTech_NMP nmp;
      this->table.insert({NMP, nmp.get_recipe()});
    }
    else if (this->type == AMBIT_ROWCLONE) {
      DRAM_AMBIT_ROWCLONE ambit_rowclone;
      this->table.insert({AMBIT_ROWCLONE, ambit_rowclone.get_recipe()});
    }
    else {
      cout << "ERROR: Recipe Table: PUM technology not available" << endl;
      exit(-1);
    }
  };

};
#endif
