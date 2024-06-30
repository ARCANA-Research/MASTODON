#include "fetcher.h"

/**
* @brief Constructor for Fetcher class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Controller class.
*/
Fetcher::Fetcher() {
  this->state = FETCH_READY;
};

/**
* @brief Connect the fetcher to a binary storage
*/
void Fetcher::connect_binary_storage(Binary_Storage * bin) {
  this->connected_bin = bin;
}

/**
* @brief Attempt to fetch an instruction
*
* This function is called by the Controller every cycle if the fetcher is idle.
* If a fetch is success, then the controller will process the instruction further, and
* the fetcher enters the FETCH_WAIT_ENTRY state
* If the fetch is not sucessful, then the fetcher still remains idle (FETCH_READY) and the controller
* has to try it again.
*
* @param PC the program counter to fetch the instruction
*
*/
void Fetcher::fetch_attempt(size_t PC) {
  char * raw_data = (char*) malloc(sizeof(char) * this->connected_bin->data_size);
  // Check if instruction has been loaded to binary storage
  if (this->connected_bin->send_to_controller(PC, raw_data) == 0) {
    this->fetched_entry =
    Data <char>(this->connected_bin->output_latency, this->connected_bin->data_size, raw_data);

    // Begin transfering the entry to the controller
    this->fetched_entry.begin_countdown();
    this->state = FETCH_WAIT_ENTRY;
  }
  else {
    this->state = FETCH_READY;
    free(raw_data);
  }
}

/**
* @brief Advance the state of the fetcher
*
* Every cycle, the fetcher does the following sequentially: (1) advance the state of the
* fetched entry (basically counting it down until it is available); (2) check the FSM.
*/
void Fetcher::tick() {
  this->fetched_entry.tick();

  switch (this->state) {
    case FETCH_READY: {
      this->state = FETCH_READY; // change by controller
      break;
    }
    case FETCH_WAIT_ENTRY: {
      char * raw_data = (char*) malloc(sizeof(char) * this->connected_bin->data_size);
      if (this->fetched_entry.read_data(raw_data) == 0) {
        this->state = FETCH_FETCHED;
      }
      else {
        this->state = FETCH_WAIT_ENTRY;
      }
      free(raw_data);
      break;
    }
    case FETCH_WAIT_DECODER: {
      this->state = FETCH_WAIT_DECODER; // change by controller
      break;
    }
    case FETCH_FETCHED: {
      this->state = FETCH_FETCHED; // change by controller
      break;
    }
  }
}
