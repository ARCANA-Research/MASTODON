#include "cluster.h"

/**
* @brief Activate the correct context
*
* @param pipeline_idx the index of the targeted pipeline to activate
*/
// void Cluster::switch_regfile(size_t pipeline_idx) {
//   this->uopQueues->switch_pipeline(this->pipelines[pipeline_idx]);
// };
//
// /**
// * @brief Recieve the micro-op from the Controller
// *
// * @param uOp the micro-op sent by the controller
// */
// void Cluster::recieve_uOp(Uop uOp) {
//   this->uopQueues->recieve_uOp(uOp);
// }

/**
* @brief Constructor for the Cluster class
*
* This is an independent module with its own tick() function that update every
* clock cycle. This class is stimulated/called by the Controller class. The constructor
* instantiate the associated Uop_Queue, along with all of the Bit_Pipeline
*
* @param granularity_ the number of tiles in a pipeline
* @param num_pipeline_ the number of pipelines inside the cluster
* @param num_col_ the number of columns inside a tile
* @param num_row_ the number of rows inside a tile
* @param num_mask_ the number of mask columns inside a tile
* @param num_imm_ the number of intermediate columns inside a tile
* @param ID_ a unique ID (number) associated with the cluster/lane
*
*/
Cluster::Cluster(string config_file_addr, size_t ID_){
  this->granularity = stoull(util::general_config_parser("granularity", config_file_addr));

  if (this->granularity % 8 != 0) {
    printf("CLUSTER: ERROR: granularity not a multiple of 8\n");
    exit(-1);
  }
  this->num_pipeline = stoull(util::general_config_parser("num_regfile", config_file_addr));
  this->num_col =  stoull(util::general_config_parser("num_col", config_file_addr));
  this->num_row = stoull(util::general_config_parser("num_row", config_file_addr));
  this->num_imm = stoull(util::general_config_parser("num_imm", config_file_addr));
  this->num_mask = stoull(util::general_config_parser("num_mask", config_file_addr));
  this->queue_size = stoull(util::general_config_parser("entry_queue_size", config_file_addr));
  this->ID = ID_;

  this->uop_queue = new Uop_Queue(this->granularity, this->queue_size);

  for (size_t p = 0; p < this->num_pipeline; p+=1) {
    this->pipelines.push_back(new Bit_Pipeline(this->granularity,
                                               this->num_row,
                                               this->num_col,
                                               this->num_mask,
                                               this->num_imm,
                                               this->ID + p*this->granularity));
  }

  this->uop_queue->connect_pipelines(this->pipelines);

  this->primitive_op_count.insert({"AND", 0});
  this->primitive_op_count.insert({"NAND", 0});
  this->primitive_op_count.insert({"OR", 0});
  this->primitive_op_count.insert({"NOR", 0});
  this->primitive_op_count.insert({"XOR", 0});
  this->primitive_op_count.insert({"NOT", 0});
  this->primitive_op_count.insert({"COPY", 0});
  this->primitive_op_count.insert({"FA", 0});
};

map <string, size_t> Cluster::report_primitive_op_count() {
  for (size_t i = 0; i < this->num_pipeline; i+=1) {
    map <string, size_t> temp = this->pipelines[i]->report_primitive_op_count();
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

void Cluster::toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL) 
{
	for (size_t p = 0; p < this->num_pipeline; p+=1) {
    		this->pipelines[p]->toggle_device_model_sim(device_model_sim, memorisation, cycle_time, volt_MAGIC, volt_ISO_BL);
  	}
}

Column * Cluster::fetch_torus_register(int pipeline_idx) {
  return this->pipelines[pipeline_idx]->buffers[this->granularity - 1];
}

Column * Cluster::fetch_regular_register(int pipeline_idx, int reg_idx, int bit_pos) {
  return &(this->pipelines[pipeline_idx]->tiles[bit_pos]->columns[reg_idx]);
}

/**
* @brief Set the record flag used to record in an XML file
*/
void Cluster::record_log(bool record_) {
  this->uop_queue->record_log(record_);
  // for (size_t i = 0; i < this->num_pipeline; i+=1) {
  //   this->pipelines[i]->record_log(record_);
  // }
};

void Cluster::show_log(bool record_) {
  this->uop_queue->record_log(record_);
  // for (size_t i = 0; i < this->num_pipeline; i+=1) {
  //   this->pipelines[i]->record_log(record_);
  // }
};

void Cluster::toggle_backend(bool disable_signal) {
	this->uop_queue->toggle_backend(disable_signal);
}

/**
* @brief Create xml node to use for record and to send to all child nodes
*/
void Cluster::set_log_node(pugi::xml_node parent_node_) {
   //std::stringstream ss;
   //ss << "Cluster_" << this->ID;
   //this->parent_node = parent_node_;
   //this->this_node = this->parent_node.append_child(ss.str().c_str());
   this->uop_queue->set_log_node(parent_node_);
   //for (size_t i = 0; i < this->num_pipeline; i+=1) {
     //this->pipelines[i]->set_log_node(this->this_node);
   //}
}

bool Cluster::is_active() {
	return this->uop_queue->is_active();
}

float Cluster::report_activity_factor() {
	return this->uop_queue->report_activity_factor();
}

/**
* @brief Write data to a specific pipeline
*
* @param pipeline_idx the index of the targeted pipeline
* @param data the data object to be written
*/
void Cluster::write_data(size_t pipeline_idx, vector <vector <string> > data) {
  this->pipelines[pipeline_idx]->write_data(data);
};

/**
* @brief Display the content of a pipeline, use for debugging
*
* This function needs to be explicitly called by whoever is trying to debug the sim.
* It is does not get called during normal simulation run
*
* @param pipeline_idx the index of the targeted pipeline
*/
void Cluster::display_data(size_t pipeline_idx) {
  this->pipelines[pipeline_idx]->display_data();
};

void Cluster::receive(vector<size_t> active_regfiles, Racer_Uop uOp) {
  this->uop_queue->receive(active_regfiles, uOp);
};

/**
* @brief Get a byte out of a pipeline.
*
* @param pipeline_idx the index of the targeted pipeline
* @param row the row position
* @param col  the col position
* @param bg which bytegroup should be returned
*
* @return res the bytegroup required
*/
uint8_t Cluster::get_byte(size_t pipeline_idx, size_t bg, size_t row, size_t col) {
    return this->pipelines[pipeline_idx]->get_byte(row, col, bg);
};

/**
* @brief Advance the state of the cluster
*
* The main purpose of the function is to advance the state of the cluster's Uop_Queue.
* It also conditionally record the state of all of the pipelines
*/
void Cluster::tick() {
  //bookeeping
  // this->uopQueues->write_to_node();
  for (size_t i = 0; i < this->num_pipeline; i+=1) {
    this->pipelines[i]->write_to_node();
  }
  this->uop_queue->tick();
};
