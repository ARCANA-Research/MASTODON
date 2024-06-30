#ifndef NODE_CPP
#define NODE_CPP
#include "node.h"

/**
* @brief Default constructor for Node (Memory Processing Unit) class
*
* @param control_group_ID_ unique ID (number) associated with the Node
*/
/*
template <class Back_End, class Decoder_Type>
Node<Back_End, Decoder_Type>::Node(size_t control_group_ID_)
{
  this->control_group_ID = control_group_ID_;
  this->num_lane = 8;
  this->map_style = IDEAL;
  this->total_entries = 100;
  this->output_latency = 1;
  this->input_latency = 1;
  this->granularity = 8;
  //this->queue_size = 16;
  this->num_regfile = 4;
  this->num_col = 8;
  this->num_row = 8;
  this->num_imm = 17;
  this->num_mask = 4;
  this->idle_counter = 0;
  this->set_show(false, "all");
  this->set_record(false, "all");
  this->mover_type = SERIAL;
  this->mover_COPY_lat = 1;
  this->mover_SETUP_lat = 1;
  this->loading_bin = false;
  this->playback_buffer_size = 0;
  this->max_num_regfile_per_lane = 1;
  this->num_thread = 1;
  this->setup(0);
};

template <class Back_End, class Decoder_Type>
Node<Back_End, Decoder_Type>::Node( size_t control_group_ID_,
                              size_t num_lane_,
                              PUMtech map_style_,
                              size_t total_entries_,
                              size_t output_latency_,
                              size_t input_latency_,
                              size_t granularity_,
                              //size_t queue_size_,
                              size_t num_regfile_,
                              size_t num_col_,
                              size_t num_row_,
                              size_t num_imm_,
                              size_t num_mask_,
                              size_t random_bit_pos_,
                              size_t mover_COPY_lat_,
                              size_t mover_SETUP_lat_,
                              size_t playback_buffer_size_,
                              size_t max_num_regfile_per_lane_,
                              size_t num_thread_,
                              DataMoverType mover_type_)
{
  this->control_group_ID = control_group_ID_;
  this->num_lane = num_lane_;
  this->map_style = map_style_;
  this->total_entries = total_entries_;
  this->output_latency = output_latency_;
  this->input_latency = input_latency_;
  this->granularity = granularity_;
  //this->queue_size = queue_size_;
  this->num_regfile = num_regfile_;
  this->num_col = num_col_;
  this->num_row = num_row_;
  this->num_imm = num_imm_;
  this->num_mask = num_mask_;
  this->idle_counter = 0;
  this->mover_COPY_lat = mover_COPY_lat_;
  this->mover_SETUP_lat = mover_SETUP_lat_;
  this->loading_bin = false;
  this->mover_type = mover_type_;
  this->playback_buffer_size = playback_buffer_size_;
  this->max_num_regfile_per_lane = max_num_regfile_per_lane_;
  this->num_thread = num_thread_;
  this->setup(random_bit_pos_);
};
*/
/**
* @brief Configurable constructor for Node (Memory Processing Unit) class
*
* @param this->config_file_addr the address of the Node configuration file
*/
template <class Back_End, class Decoder_Type>
Node<Back_End, Decoder_Type>::Node(size_t control_group_ID_, string config_file_addr_) {
  this->control_group_ID = control_group_ID_;
  this->config_file_addr = config_file_addr_;
  this->num_lane = stoull(util::general_config_parser("num_lane", this->config_file_addr));
  string tech = util::general_config_parser("map_style", this->config_file_addr);
  if (tech == "MAGIC") {
    this->map_style = MAGIC;
  }
  if (tech == "FELIX") {
    this->map_style = FELIX;
  }
  if (tech == "OSCAR") {
    this->map_style = OSCAR;
  }
  if (tech == "NMP") {
    this->map_style = NMP;
  }
  if (tech == "IDEAL") {
    this->map_style = IDEAL;
  }
  if (tech == "AMBIT_ROWCLONE") {
    this->map_style = AMBIT_ROWCLONE;
  }
  string mov = util::general_config_parser("mover_type", this->config_file_addr);
  if (mov == "SERIAL") {
    this->mover_type = SERIAL;
  }
  else if (mov == "RING") {
    this->mover_type = RING;
  }
  else if (mov == "P2P") {
    this->mover_type = P2P;
  }  string disable_signal = util::general_config_parser("disable_backend", this->config_file_addr);
  if (disable_signal.find("true") != std::string::npos) {
    this->disable_signal = true;
  }
  else {
    this->disable_signal = false;
  }
  string device_model_sim = util::general_config_parser("device_model_sim", config_file_addr);
  string memorisation = util::general_config_parser("memorisation", config_file_addr);
  if (device_model_sim.find("true") != std::string::npos) 
  {
    this->device_model_sim = true;
    if (memorisation.find("true") != std::string::npos) 
    {
      this->memorisation = true;
    }
    else
    {
      this->memorisation = false;
    }
  }
  else 
  {
    this->device_model_sim = false;
    this->memorisation = false;
  }
  this->total_entries = stoull(util::general_config_parser("entry_bin_size", this->config_file_addr));
  this->output_latency = stoull(util::general_config_parser("cycle_bin_out_lat", this->config_file_addr));
  this->input_latency = stoull(util::general_config_parser("cycle_bin_in_lat", this->config_file_addr));
  this->granularity = stoull(util::general_config_parser("granularity", this->config_file_addr));
  //this->queue_size = stoull(util::general_config_parser("entry_queue_size", this->config_file_addr));
  this->num_regfile = stoull(util::general_config_parser("num_regfile", this->config_file_addr));
  this->num_col = stoull(util::general_config_parser("num_col", this->config_file_addr));
  this->num_row = stoull(util::general_config_parser("num_row", this->config_file_addr));
  this->num_imm = stoull(util::general_config_parser("num_imm", this->config_file_addr));
  this->num_mask = stoull(util::general_config_parser("num_mask", this->config_file_addr));
  size_t random_bit_pos = stoull(util::general_config_parser("random_bit_pos", this->config_file_addr));
  this->mover_COPY_lat = stoull(util::general_config_parser("cycle_mover_COPY_lat", this->config_file_addr));
  this->mover_SETUP_lat = stoull(util::general_config_parser("cycle_mover_SETUP_lat", this->config_file_addr));
  this->playback_buffer_size = stoull(util::general_config_parser("entry_playback_buffer_size", this->config_file_addr));
  this->max_num_regfile_per_lane = stoull(util::general_config_parser("num_max_active_regfile_per_lane", this->config_file_addr));
  this->num_thread = stoull(util::general_config_parser("num_smt_thread", this->config_file_addr));
  this->cycle_time = (util::general_config_parser("second_cycle_time", config_file_addr));
  this->volt_MAGIC = (util::general_config_parser("volt_MAGIC", config_file_addr));
  this->volt_ISO_BL = (util::general_config_parser("volt_ISO_BL", config_file_addr));

  this->loading_bin = false;
  this->idle_counter = 0;
  this->setup(random_bit_pos);
}

/**
* @brief Instantiate Binary_Storage, Controller, and Cluster (lanes)
*
* @param for all tiles, the first random_bit_pos are random intially, the rest are 0
*/
template <class Back_End, class Decoder_Type>
void Node<Back_End, Decoder_Type>::setup(size_t random_bit_pos)
{
  this->activity_factor = 0.0;

  this->primitive_op_count.clear();
  this->primitive_op_count.insert({"AND", 0});
  this->primitive_op_count.insert({"NAND", 0});
  this->primitive_op_count.insert({"OR", 0});
  this->primitive_op_count.insert({"NOR", 0});
  this->primitive_op_count.insert({"XOR", 0});
  this->primitive_op_count.insert({"NOT", 0});
  this->primitive_op_count.insert({"COPY", 0});
  this->primitive_op_count.insert({"FA", 0});

  // Create controller
  this->controller = new Controller<Back_End, Decoder_Type> (this->control_group_ID,
                                     this->num_lane,
                                     this->granularity,
                                     this->playback_buffer_size, 
                                     this->max_num_regfile_per_lane,
                                     this->num_thread,
                                     this->map_style
                                    );

  // Create binary storage
  this->bin = new Binary_Storage(this->control_group_ID,
                                 this->total_entries,
                                 this->output_latency,
                                 this->input_latency,
                                 this->data_size
                                );

  this->controller->connect_binary_storage(bin);

  // Create lanes
  this->lanes.clear();
  for (size_t i = 0; i < num_lane; i+=1) {
    Back_End * lane = new Back_End(this->config_file_addr,i);
    
    if (random_bit_pos != 0) {
	      util::init_lane_random(lane, random_bit_pos);
	    }
    this->lanes.push_back(lane);	             
  }

  this->controller->connect_lanes(lanes);

  for (size_t i = 0; i < this->num_lane; i += 1) {
    this->lanes[i]->toggle_backend(this->disable_signal);
    this->lanes[i]->toggle_device_model_sim(this->device_model_sim, this->memorisation, this->cycle_time, this->volt_MAGIC, this->volt_ISO_BL);
  }


  // Create data mover
  if (this->mover_type == SERIAL) {
    this->mover = new Serial_Mover<Back_End>(this->mover_COPY_lat, this->mover_SETUP_lat);
    this->mover->connect_lanes(lanes);
  }
  else if (this->mover_type == RING) {
    this->mover = new Ring_Mover<Back_End>(this->mover_COPY_lat, this->mover_SETUP_lat);
    this->mover->connect_lanes(lanes);
  }
  else if (this->mover_type == P2P) {
    this->mover = new P2P_Mover<Back_End>(this->mover_COPY_lat, this->mover_SETUP_lat);
    this->mover->connect_lanes(lanes);
  }
  else {
    printf("ERROR: Node Mover does not exist\n");
    exit(-1);
  }
  this->controller->connect_mover(this->mover);
};

/**
* @brief Load an entire binary file into the Node's binary storage
*
* @param binary_file the address to the binary file
*/
template <class Back_End, class Decoder_Type>
int Node<Back_End, Decoder_Type>::load_to_bin(string binary_file) {
  if (this->loading_bin == true) {
    return -1;
  }
  else {
    this->loading_bin = true;
    this->opened_bin_file = fopen(binary_file.c_str(), "r");
    fgets(this->entry_to_store, BIN_ENTRY_SIZE + 1, this->opened_bin_file);
  }
  return 0;
};
template <class Back_End, class Decoder_Type>
map <string, size_t> Node<Back_End, Decoder_Type>::report_primitive_op_count() {
  for (size_t i = 0; i < this->num_lane; i+=1) {
    map <string, size_t> temp = this->lanes[i]->report_primitive_op_count();
    this->primitive_op_count["AND"] += temp["AND"];
    this->primitive_op_count["NAND"] += temp["NAND"];
    this->primitive_op_count["OR"] += temp["OR"];
    this->primitive_op_count["NOR"] += temp["NOR"];
    this->primitive_op_count["XOR"] += temp["XOR"];
    this->primitive_op_count["NOT"] += temp["NOT"];
    this->primitive_op_count["COPY"] += temp["COPY"];
    this->primitive_op_count["FA"] += temp["FA"];
  }
  return this->primitive_op_count;
}

/**
* @brief Set the record flag used to record in an XML file
*
* @param flag set the field flags to either true or false
* @param fields all field flags that need to be set specify in a single comma-seperated string. The avaialble fields currently
* are controller, binary, lane<0> to lane<N>. The fields can be specified out of order
*/
template <class Back_End, class Decoder_Type>
void Node<Back_End, Decoder_Type>::set_record(bool flag, string fields) {
  if (fields == "all") {
    this->controller->record_log(flag);
    this->bin->record_log(flag);
    this->mover->record_log(flag);
    this->controller->decoder->record_log(flag);
#if SST_ENABLED
    this->controller->mpi_server->record_log(flag);
#endif
    // this->controller->record_log(flag);
    for (size_t i = 0; i < this->num_lane; i+=1) {
      this->lanes[i]->record_log(flag);
    }
  }
  else if (fields == "none") {
    this->controller->record_log(!flag);
    this->bin->record_log(!flag);
    this->mover->record_log(!flag);
    this->controller->decoder->record_log(!flag);
#if SST_ENABLED
    this->controller->mpi_server->record_log(!flag);
#endif
    // this->controller->record_log(!flag);
    for (size_t i = 0; i < this->num_lane; i+=1) {
      this->lanes[i]->record_log(!flag);
    }
  }
  else {
    vector<string> output;
    istringstream ss(fields);
    while (ss) {
        string token;
        if (!getline(ss, token, ',')) break;

        istringstream ss_token(token);
        while (ss_token) {
            string subtoken;
            if (!getline(ss_token, subtoken, ' ')) break;
            output.push_back(subtoken);
        }
    }

    for (size_t i = 0; i < output.size(); i+=1) {
      string field = output[i];
      if (field == "controller") {
        this->controller->record_log(flag);
      }
      else if (field == "binary") {
        this->bin->record_log(flag);
      }
      else if (field == "decoder") {
        this->controller->decoder->record_log(flag);
      }
      else if (field == "intra_mover") {
        this->mover->record_log(flag);
      }
      else if (field.find("lane") != std::string::npos) {
        size_t last_digit_index = field.find_first_of("0123456789");
        int idx = std::stoi(field.substr(last_digit_index));
        this->lanes[idx]->record_log(flag);
      }
#if SST_ENABLED
      else if (field.find("inter_mover") != std::string::npos) {
        this->controller->mpi_server->record_log(flag);
      }
#endif
    }
  }
};

/**
* @brief Set the show flag used to record in an XML file
*
* @param flag set the field flags to either true or false
* @param fields all field flags that need to be set specify in a single comma-seperated string. The avaialble fields currently
* are controller, binary, lane<0> to lane<N>. The fields can be specified out of order
*/
template <class Back_End, class Decoder_Type>
void Node<Back_End, Decoder_Type>::set_show(bool flag, string fields) {
  if (fields == "all") {
    this->controller->show_log(flag);
    this->bin->show_log(flag);
    this->mover->show_log(flag);
    this->controller->decoder->show_log(flag);
#if SST_ENABLED
    this->controller->mpi_server->show_log(flag);
#endif
    // this->controller->show(flag);
    for (size_t i = 0; i < this->num_lane; i+=1) {
      this->lanes[i]->show_log(flag);
    }
  }
  else if (fields == "none") {
    this->controller->show_log(!flag);
    this->bin->show_log(!flag);
    this->controller->decoder->show_log(!flag);
    this->mover->show_log(!flag);
#if SST_ENABLED
    this->controller->mpi_server->record_log(!flag);
#endif
    // this->controller->show(!flag);
    for (size_t i = 0; i < this->num_lane; i+=1) {
      this->lanes[i]->show_log(!flag);
    }
  }
  else {
    vector<string> output;
    istringstream ss(fields);
    while (ss) {
        string token;
        if (!getline(ss, token, ',')) break;

        istringstream ss_token(token);
        while (ss_token) {
            string subtoken;
            if (!getline(ss_token, subtoken, ' ')) break;
            output.push_back(subtoken);
        }
    }

    for (size_t i = 0; i < output.size(); i+=1) {
      string field = output[i];
      if (field == "controller") {
        this->controller->show_log(flag);
      }
      else if (field == "binary") {
        this->bin->show_log(flag);
      }
      else if (field == "decoder") {
        this->controller->decoder->show_log(flag);
      }
      else if (field == "intra_mover") {
        this->mover->show_log(flag);
      }
      else if (field.find("lane") != std::string::npos) {
        size_t last_digit_index = field.find_last_of("0123456789");
        int idx = std::stoi(field.substr(last_digit_index));
        this->lanes[idx]->show_log(flag);
      }
#if SST_ENABLED
      else if (field.find("inter_mover") != std::string::npos) {
        this->controller->mpi_server->show_log(flag);
      }
#endif
    }
  }
};

/**
* @brief Create xml node to use for record and to send to all child nodes
*/
template <class Back_End, class Decoder_Type>
void Node<Back_End, Decoder_Type>::set_log_node(pugi::xml_node node) {
  this->bin->set_log_node(node);
  this->controller->set_log_node(node);
  this->mover->set_log_node(node);
  // this->controller->set_log_node(node);
  for (size_t i = 0; i < this->num_lane; i+=1) {
    this->lanes[i]->set_log_node(node);
  }
}
template <class Back_End, class Decoder_Type>
float Node<Back_End, Decoder_Type>::report_activity_factor(size_t num_clock_cycle) {
  return this->activity_factor / (float) num_clock_cycle;
};

/**
* @brief Advance the state of the Node
*
* The Node ticks all of its sub modules, and also manage the loading of the binary file
*/
template <class Back_End, class Decoder_Type>
void Node<Back_End, Decoder_Type>::tick() {
  // Keep loading until there is nothing left to be loaded
  if (this->loading_bin == true) {
    if (this->bin->store(this->entry_to_store, BIN_ENTRY_SIZE) != -1) {
      bool close_condition = fgets(this->entry_to_store, BIN_ENTRY_SIZE + 1, this->opened_bin_file) == NULL;
      if (close_condition) {
        this->loading_bin = false;
        fclose(this->opened_bin_file);
      }
      else {
        this->loading_bin = true;
      }
    }
  }

  // Tick all sub module
  this->bin->tick();
  this->controller->tick();
  for (size_t i = 0; i < this->num_lane; i+=1) {
    this->lanes[i]->tick();
  }
#if SST_ENABLED
  if (!this->controller->is_backend_active() && !this->mover->is_active() && !this->controller->mpi_server->is_active()) {
  //if (!this->controller->is_backend_active() && !this->mover->is_active()) {
    this->idle_counter++;
  }
#else
  if (!this->controller->is_backend_active() && !this->mover->is_active()) {
    this->idle_counter++;
  }
#endif
  else {
    this->idle_counter = 0;
  }

  this->activity_factor += this->controller->report_activity_factor();
};
#endif
