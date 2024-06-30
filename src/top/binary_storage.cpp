#include "binary_storage.h"
/**
* @brief Constructor for Binary_Storage class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Node class. The class has three
* main functions: (1) recieve entries from external (i.e., hdd, other main memory devices);
* (2) respond to a controller's fetch; (3) delete an entry.
*
* @param ID_ a unique ID (number) associated with the binary storage
* @param total_entries_ the maximum number of entries that can be stored
* @param output_latency_ the number of cycles needed for controller to fetch an entry from the binary storage
* @param input_latency_ the number of cycles needed for an entry to be stored into the binary storage
* @param data_size_ the number of bytes in an entry (automatically set to 22 bytes inside Node.h)
*
*/
Binary_Storage::Binary_Storage(size_t ID_, size_t total_entries_, size_t output_latency_, size_t input_latency_, size_t data_size_)
{
  this->ID = ID_;
  this->output_latency = output_latency_;
  this->input_latency = input_latency_;
  this->data_size = data_size_;
  this->total_entries = total_entries_;
  this->num_free_entries = total_entries_;
  this-> time = 0;

  // Load Related
  this->input_busy = false;
  this->input_done_time = 0;
  this->input_start_idx = 0;
  // Store Related
  this->output_busy = false;

  this->delete_busy = false;
  this->record = false;
  this->show = false;
};

/**
* @brief Set the show flag used to printout to terminal
*/
void Binary_Storage::show_log(bool show_) {
  this->show = show_;
}

/**
* @brief Set the record flag used to record in an XML file
*/
void Binary_Storage::record_log(bool record_) {
  this->record = record_;
}

/**
* @brief Create xml node to use for record and to send to all child nodes
*/
void Binary_Storage::set_log_node(pugi::xml_node parent_node_) {
  this->parent_node = parent_node_;
  this->this_node = this->parent_node.append_child(("Binary_Storage_" + to_string(this->ID)).c_str());
}

/**
* @brief Store a binary file into the binary storage
*
* This function allocates a consecutive block of entries and
* sequentially get one entry at a time. In practice, we only store one entry at
* a time (this can be changed if we decide to change the HW architecture). See the
* Node::tick() function for more details. In theory, if we want to store multiple entries
* using a single store call, we can. The function will automatically perform the allocation,
* create a Data<T> object for each entry and perform the correct count down until every
* entry is available for use.
*/
int Binary_Storage::store(char* binary_entry, size_t entry_size)
{
  if (this->show) {printf("Binary Storage: attempting to store: %s\n", binary_entry);}
  // Check if store port is busy
  if(this->input_busy){
    if (this->show) {printf("Binary Storage: WARNING: store port is busy, please try later\n");}
    return -1;
  }

  // Check if currently deleting
  if(this->delete_busy){
    if (this->show) {printf("Binary Storage: WARNING: deleting entries, please try later\n");}
    return -1;
  }

  if (this->show) {printf("Binary Storage: storing: %s\n", binary_entry);}
  this->write_to_node(binary_entry);

  // Check if there are enough free entries
  size_t num_entries_needed = entry_size / this->data_size;
  if (entry_size % this->data_size != 0) {
    num_entries_needed++; // left over bytes
  }
  if (num_entries_needed > num_free_entries) {
    printf("Binary Storage: ERROR: there are not enough entries to store program, please delete()\n");
    //printf("num_entries_needed: %d num_free_entries: %d\n",num_entries_needed,num_free_entries);
    exit(-1);
  }

  this->input_busy = true;

  // Set start store cycle
  this->input_start_time = this->time;

  // Set done store cycle;
  this->input_done_time = this->time + num_entries_needed * this->input_latency - 1;

  // Store data in binary storage
  for (size_t i = 0; i < num_entries_needed; i += 1) {
    Data<char> entry(this->input_latency, this->data_size, binary_entry + i * this->data_size);
    stored_data.push_back(entry);
  }

  return 0;
};

/**
* @brief Delete a consecutive number of entries in the binary storage, starting from the last entry
*
* @param delete_size number of entries to delete
*/
int Binary_Storage::delete_entry(size_t delete_size)
{
  if (input_busy || output_busy) { // why input or output?
    if (this->show) {printf("Binary Storage: WARNING: I/O are busy, cannot delete\n");}
    return -1;
  }

  if (delete_size > this->input_start_idx) {
    if (this->show) {printf("Binary Storage: WARNING: cannot delete this many entries\n");}
    return -1;
  }

  this->stored_data.erase(this->stored_data.end() - delete_size, this->stored_data.end());
  this->input_start_idx -= delete_size;
  this->num_free_entries += delete_size;
  this->delete_busy = true;
  return 0;
};

/**
* @brief Print out the content of the binary storage
*/
void Binary_Storage::print_storage()
{
  //std::cout << "================= CYCLE: " << this->time << " =================" << std::endl;
  for (size_t i = 0; i < this->stored_data.size(); i+=1) {
    char * entry = (char*) malloc(sizeof(char) * this->data_size);
    if (this->stored_data[i].read_data(entry) == -1) {
      std::cout << i << ": <not available>" << std::endl;
    }
    else {
      std::cout << i << ": " << entry << std::endl;
    }
    free(entry);
  }
}

/**
* @brief Create a Data<T> object and send it to the controller
*
* If the function returns 0, then the Controller can create a new Data<T> to start
* counting down. See Fetcher::fetch_attempt(size_t PC) functions for more details.
*
* @param pc the programing counter (entry address)
* @param raw_data the data string containing the RACER ISA being fetched
* @return -1 the required PC has not arrived at the binary storage yet
* @return 0 the PC entry is available and can be fetched by the controller
*/
int Binary_Storage::send_to_controller(size_t pc, char* raw_data)
{
  if (pc >= this->stored_data.size()) {
    if (this->show) {printf("Binary Storage: WARNING: Requesting entry that doesn't exist yet, wait\n");}
    return -1;
  }

  // Check if data is fully stored
  Data <char> entry = this->stored_data[pc];
  if (entry.read_data(raw_data) == 0){
    return 0;
  }
  return -1;
};

/**
* @brief Write message to xml node
*/
void Binary_Storage::write_to_node(char * raw) {
  if (this->record) {
    string entry = raw;
    this->this_node.append_child("Store").text() = entry.erase(entry.length() - 1).c_str();
  }
}

/**
* @brief Advance the state of the binary storage by one cycle
*/
void Binary_Storage::tick()
{

  // Check if stored_data vector is larger than binary storage capacity
  /*cout << this->stored_data.size() << " " << this->total_entries << endl;
  if ((size_t) this->stored_data.size() > (size_t) this->total_entries) {
    printf("Binary Storage: ERROR: allocating more than binary storage capacity \n");
    exit(-1);
  }*/

  // Store related
  if (this->input_busy) {
    // If it is time to trigger a countdown
    if (this->time == (this->input_start_time + this->input_latency * (this->input_start_idx - (this->total_entries - this->num_free_entries)))) {
      this->stored_data[this->input_start_idx].begin_countdown();
      this->input_start_idx++;
    }
  }

  // Tick all data stored
  for (size_t i=0; i < this->stored_data.size(); i++) {
    this->stored_data[i].tick();
  }

    // Reset store signal
  if (this->time == this->input_done_time) {
    this->input_busy = false;
    this->num_free_entries = this->total_entries - this->input_start_idx; // are we fine with the assumption that there is no reuse ?
  }

  if (this->delete_busy) {
    this->delete_busy = false; // delete only lasts one cycle
  }

  // Increment time;
  this->time++;
};
