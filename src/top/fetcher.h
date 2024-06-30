#ifndef FETCHER_H
#define FETCHER_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <vector>

#include "pugixml.hpp"
#include "data.h"
#include "binary_storage.h"

using namespace std;

/// Finite State Machine (FSM) for the fetcher
enum fetcherState{FETCH_READY, ///< Idle state, will change to FETCH_WAIT_ENTRY by the controller
                   FETCH_WAIT_ENTRY, ///< Check every cycle to see if the entry is ready, if it is go to FETCH_FETCHED
                   FETCH_WAIT_DECODER, ///< wait indefinitely until the decoder is ready, state changed by controller
                   FETCH_FETCHED ///< instruction is ready for decode, state changed by controller
                 };

class Fetcher
{
public:
  fetcherState state;
  Data <char> fetched_entry;

  // Member functions
  Fetcher();
  void connect_binary_storage(Binary_Storage * bin);
  void fetch_attempt(size_t PC);
  void tick();
private:
  Binary_Storage * connected_bin;
};
#endif
