#ifndef DRAM_AMBIT_ROWCLONE_H
#define DRAM_AMBIT_ROWCLONE_H

#include <vector>
#include <deque>
#include <map>
#include <utility>
#include <string>

using namespace std;

class DRAM_AMBIT_ROWCLONE
{
public:
  DRAM_AMBIT_ROWCLONE(){};

  map <string, deque<string> > get_recipe() {
    map <string, deque<string> > table;
    
    /* ====================== ADD ============================= */
    deque <string> add {{
      "RCLONE C0 B_DCC1",
      "RCLONE B_DCC1 B_T0_T1_T2",
      "RCLONE A B_T2_T3",
      "RCLONE B B_DCC1",
      "MAJOR B_DCC1_T0_T3",
      "RCLONE B_DCC1b B_T0_T3",
      "MAJOR B_T0_T1_T2",
      "RCLONE B B_T1",
      "MAJOR B_T1_T2_T3 C",
      "SUBi BITPOS 1",
      "BNZE BITPOS 1",
      "DONE"
    }};
    table.insert({"ADD", add});
    
    /* ====================== MUL8 ============================= */
    deque <string> mul8 {{
      "SET_LOOPCOUNTER 332",
      "SUBi LOOPCOUNTER 1",
      "BNZE LOOPCOUNTER 1",
      "DONE"
    }};
    table.insert({"MUL8", mul8});
    
    return table;
  };
};

#endif
