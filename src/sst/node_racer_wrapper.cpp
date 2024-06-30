#include "node_racer_wrapper.h"

using namespace SST;
using namespace SST::Merlin;
using namespace SST::Interfaces;
using namespace Mastodon;

Node_Racer_Wrapper::Node_Racer_Wrapper(SST::ComponentId_t id, SST::Params& params) : Component(id) {

  // ==================================== Parameters ========================================
  this->acknowledgement_received = false;
  this->ping_received = false;
  this->interupt_ping_received = false;
  this->ping_counter = 0;
  this->store_ready = false;
  this->store_back_counter = 0;
  this->num_vns = 1; // number of virtual network should be 1 for now, can change in the future
	// SST output stream
	this->sstout = new SST::Output("", 1, 0, SST::Output::STDOUT);

	//this->Node_ID = static_cast<size_t>(static_cast<uint64_t>(id));
  this->network_id = params.find<int64_t> ("network_id", 1);
  this->num_peers = params.find<int64_t> ("num_peers", 1);
  
  for (int64_t i = 0; i < this->num_peers; i+=1) {
  	this->barrier_status.push_back(false);
  }
  this->num_ping_received = 0;

	// Seting up Node
	this->sstout->verbose(CALL_INFO, 1, 0, "Setting up Node %lu\n", this->network_id);
  bool config_found;
	this->config_file = params.find<string>("config_file", "empty", config_found);
  if (!config_found) {
     this->sstout->fatal(CALL_INFO, -1, "Error: the input did not specify 'config_file' parameter\n");
  }

  this->mpi_send_store_back_lat = stoull(util::general_config_parser("cycle_mpi_send_store_back_lat", this->config_file));
  this->mpi_interupt_store_back_lat = stoull(util::general_config_parser("cycle_mpi_interupt_store_back_lat", this->config_file));
  this->ping_skip_duration = stoull(util::general_config_parser("cycle_ping_skip_duration", this->config_file));
  this->interupt_serve_wait_duration = stoull(util::general_config_parser("cycle_interupt_serve_wait", this->config_file));
  this->interupt_wait_counter = this->interupt_serve_wait_duration;
  this->max_idle = params.find<int64_t> ("max_idle", 1000);
  this->sstout->verbose(CALL_INFO, 1, 0, "Configuration Recieved \n");
  this->node = new Node<Cluster, Racer_Decoder> (this->network_id, this->config_file);
  this->sstout->verbose(CALL_INFO, 1, 0, "Node created \n");

  this->link_control = loadUserSubComponent<SST::Interfaces::SimpleNetwork> 
    ("networkIF", ComponentInfo::SHARE_NONE, this->num_vns);

  if (!(this->link_control)) {
      // Just load the default
      Params if_params;
      if_params.insert("link_bw",params.find<std::string>("link_bw"));
      if_params.insert("input_buf_size",params.find<std::string>("buffer_length","1kB"));
      if_params.insert("output_buf_size",params.find<std::string>("buffer_length","1kB"));
      if_params.insert("port_name","rtr");
      this->link_control = loadAnonymousSubComponent<SST::Interfaces::SimpleNetwork>
          ("merlin.linkcontrol", "networkIF", 0,
           ComponentInfo::SHARE_PORTS | ComponentInfo::INSERT_STATS, if_params, this->num_vns /* vns */);
  }

  // ==================================== SST Registration ========================================
  // tell SST that we will handle when to exit the sim
  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();

  // ==================================== Clock Registration ========================================
  string frequency = util::general_config_parser("MHz_frequency", this->config_file);
  // register the clock function so SST can increment it
  this->clock_functor = new SST::Clock::Handler<Node_Racer_Wrapper>(this, &Node_Racer_Wrapper::clockTic); 
  registerClock(frequency + "MHz", this->clock_functor, false);
  this->sstout->verbose(CALL_INFO, 1, 0, "Node clock registered \n");

  // ==================================== Link Registration ========================================
  //this->link_control->setNotifyOnReceive(new SST::Interfaces::SimpleNetwork::Handler<Node_Racer_Wrapper>(this, &Node_Racer_Wrapper::receive_event));
  this->send_notify_functor = new SST::Interfaces::SimpleNetwork::Handler<Node_Racer_Wrapper>(this, &Node_Racer_Wrapper::send_notify);

  // ==================================== Load Binary Program ========================================
  size_t num_lane = stoull(util::general_config_parser("num_lane", this->config_file));
  string data_source = util::general_config_parser("data_source", this->config_file);
  if (data_source.find("none") == std::string::npos) {
    //util::init_node_user_input(node->lanes, num_lane, data_source);
    util::read_data_from_file(node->lanes,data_source);
    this->sstout->verbose(CALL_INFO, 1, 0, "Initial data loaded\n");
  }
  node->load_to_bin(params.find<string>("binary_file", "empty"));
  this->sstout->verbose(CALL_INFO, 1, 0, "Binary program  loaded\n");

  // ==================================== Configure MPI Server ========================================
  this->mpi_server = new MPI_Server(id, // dont use this id for any purpose, this is assigned by SST 
                                    stoull(util::general_config_parser("cycle_mpi_send_payload_generation_lat", this->config_file)), 
                                    stoull(util::general_config_parser("cycle_mpi_interupt_payload_generation_lat", this->config_file)), 
                                    stoull(util::general_config_parser("bit_payload_size", this->config_file)), 
                                    stoull(util::general_config_parser("bit_PING_size", this->config_file)), 
                                    stoull(util::general_config_parser("bit_ACK_size", this->config_file)), 
                                    stoull(util::general_config_parser("bit_DONE_size", this->config_file)), 
                                    this->network_id,
                                    this->link_control);
                                    //this->send_notify_functor);
  this->mpi_server->connect_lanes(this->node->lanes);
  this->node->controller->connect_mpi_service(this->mpi_server);
  this->sstout->verbose(CALL_INFO, 1, 0, "MPI Server created\n");

  // ==================================== Bookeeping ========================================
  node->set_show(true, util::general_config_parser("show", this->config_file));
  this->record_element = util::general_config_parser("record_stat", this->config_file);
  node->set_record(true, this->record_element);

  // Setting up xml if applicable
  if (this->record_element.find("none") == std::string::npos) {
      this->stat_root = this->stat_doc.append_child("root");
  }
  string record_data = util::general_config_parser("record_data", this->config_file);
  if (record_data.find("false") == std::string::npos) {
  	this->data_root = this->data_doc.append_child("root");
  	pugi::xml_node data_in = this->data_root.append_child("in");
  	this->record_node_data(data_in);
  }
};

Node_Racer_Wrapper::~Node_Racer_Wrapper() {
    delete this->link_control;
};

void Node_Racer_Wrapper::init(unsigned int phase) {
    this->link_control->init(phase);
    
};

void Node_Racer_Wrapper::setup() {
    this->link_control->setup();
};

void Node_Racer_Wrapper::finish() {
    this->link_control->finish();
    if (this->record_element.find("none") == std::string::npos) {
	string xml_out = "./stat." + to_string(this->network_id) + ".xml";
	this->stat_doc.save_file(xml_out.c_str());
    }
    string record_data = util::general_config_parser("record_data", this->config_file);
    if (record_data.find("false") == std::string::npos) {
  	pugi::xml_node data_out = this->data_root.append_child("out");
  	this->record_node_data(data_out);
  	string xml_out = "./data." + to_string(this->network_id) + ".xml";
	this->data_doc.save_file(xml_out.c_str());
    }
};

bool Node_Racer_Wrapper::send_notify(int vn) {
  registerClock(this->clock_tc, this->clock_functor);
  return false;
}

void Node_Racer_Wrapper::receive_event() {
  if (this->link_control->requestToReceive(0) && this->mpi_server->is_active()) {
	  SimpleNetwork::Request* req = this->link_control->recv(0);
	  Node_Event * event = dynamic_cast<Node_Event*>(req->takePayload());

	  // check if I am the correct receiver (except for MPI_BARRIER)
	  if ((event->des_network_id != this->network_id) && this->mpi_server->state != MPI_BARRIER_WAIT4PING) {
	    this->sstout->fatal(CALL_INFO, -1, "Error: Received unintended package\n");
	  }
	  
	  // if it comes from unintended src, ignore
	  this->sstout->verbose(CALL_INFO, 1, 0, "Node %lu received a package type %d from Node %lu\n", this->network_id, event->type, event->src_network_id);
	  
	  // Handle SEND/RECV/INTRP events
	  if (this->mpi_server->state != MPI_BARRIER_WAIT4PING) {
	          // Acknowledgement
		  if (event->type == ACK && event->src_network_id == this->mpi_server->des_network_id) {
		    this->acknowledgement_received = true;
		    this->receiving_ack_from = event->src_network_id;
		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "ACK " + to_string(this->receiving_ack_from));
		  }
		  // Regular Ping
		  if (event->type == PING && event->src_network_id == this->mpi_server->src_network_id) {
		    this->ping_received = true;
		    this->receiving_ping_from = event->src_network_id;

		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "PING " + to_string(this->receiving_ping_from));
		  }
		  // Interupt Ping
		  if (event->type == INTRP_PING) {
		    this->interupt_ping_received = true;
		    this->receiving_ping_from = event->src_network_id;
		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "INTRP_PING " + to_string(this->receiving_ping_from));
		  }
		  // Data transfer
		  if (event->type == SEND && event->src_network_id == this->mpi_server->src_network_id) {
		    this->store_back_counter += this->mpi_send_store_back_lat;
		    this->recv_payload_queue.push(event);

		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "Payload " + to_string(event->src_network_id));
		  }
		  // Interupt instructions
		  if (event->type == INTRP ) {
		    this->store_back_counter += this->mpi_interupt_store_back_lat;
		    this->recv_payload_queue.push(event);

		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "Payload " + to_string(event->src_network_id));
		  }
		  // Shutdown
		  if (event->type == DONE) {
		    this->store_ready = true;
		    //this->mpi_server->shutdown();
		    //bookeeping, not important,
		    this->mpi_server->write_to_node("Receive_Event", "Shutdown " + to_string(event->src_network_id));
		  }
	  }
	  // Handle Barrier Events
	  if (this->mpi_server->state == MPI_BARRIER_WAIT4PING && event->type == PING) {
	  	int64_t src_network_id = event->src_network_id;
	  	if (this->barrier_status[src_network_id] == false) {
	  		this->barrier_status[src_network_id] = true;
	  		this->num_ping_received ++;
	  	}
	  }
	  delete req;
  }
}

void Node_Racer_Wrapper::store_payload(Node_Event * payload) {

    if (payload->type != SEND && payload->type != INTRP) {
    	printf("ERROR: MPI Server: can only store payload of event type SEND or INTRP, trying to store type %d\n", payload->type);
    	exit(-1);
    }

    if (payload->type == SEND) {
	    size_t des_lane = payload->des_lane;
	    size_t des_regfile = payload->des_regfile;
	    size_t des_regaddr = payload->des_regaddr;
	    vector <size_t> data = payload->src_data;

	    size_t num_bit = this->node->granularity;
	    size_t col_size = this->node->num_row;
	    vector <string> binary_data;
	    for (unsigned int i = 0; i < data.size(); i+=1) {
    		string row = bitset<sizeof(size_t) * 8>(data[i]).to_string();
    		row = row.substr(row.length() - num_bit);
    		binary_data.push_back(row);
	    }

	    vector<string> transposed_binary_data;
	    transposed_binary_data.resize(binary_data[0].size());
	    // Iterate through each row in the original matrix
	    for (const string& row : binary_data) {
    		// Iterate through each element in the row and append it to the corresponding column in transposedMatrix
    		for (size_t col = 0; col < row.size(); ++col) {
    		    transposed_binary_data[col] += row[col];
    		}
	    }
	    for (unsigned int i = 0; i < transposed_binary_data.size(); i+=1) {
    		Column * col = new Column(transposed_binary_data[transposed_binary_data.size() - 1 - i], col_size);
    		this->node->lanes[des_lane]->pipelines[des_regfile]->tiles[i]->columns[des_regaddr].copy(col);
  	  }
    }
    else {
    	this->node->controller->interupt_request_queue.push(payload->interupt_instruction);
    }
}

void Node_Racer_Wrapper::store_back() {
    if (this->store_back_counter > 0) {
      this->store_back_counter -= 1;
    }
    else {
      this->acknowledgement_received = false;
      this->ping_received = false;
      this->interupt_ping_received = false;
      this->store_ready = false;
      this->store_back_counter = 0;
      this->node->controller->decoder->state = DECODER_READY;

      while(!this->recv_payload_queue.empty()) {
        this->store_payload(this->recv_payload_queue.front());
        this->recv_payload_queue.pop();
      }
      this->mpi_server->shutdown();
    }
};

void Node_Racer_Wrapper::record_node_data(pugi::xml_node node) {
   for (int l = 0; l < this->node->num_lane; l+=1) {
    pugi::xml_node lane_node = node.append_child(("L" + to_string(l)).c_str());
    for (int f = 0; f < this->node->num_regfile; f+=1) {
      pugi::xml_node regfile_node = lane_node.append_child(("RF" + to_string(f)).c_str());
      for (int c = 0; c < this->node->num_col; c+=1) {
        vector <size_t> output = util::fetch_column(this->node->lanes[l], f, c);
        string data = "";
        for (int r = 0; r < this->node->num_row; r+=1) {
          data += to_string(output[r]);
          data += " ";
        }
        regfile_node.append_child(("C" + to_string(c)).c_str()).text() = data.c_str();
      }
    }
  }
};

bool Node_Racer_Wrapper::clockTic(SST::Cycle_t clk) {
  if (this->record_element.find("none") == std::string::npos) {
      std::stringstream ss;
      ss << "Cycle_" << clk;
      pugi::xml_node clk_node = this->stat_root.append_child(ss.str().c_str());
      this->node->set_log_node(clk_node);
      this->mpi_server->set_log_node(clk_node);
  }
  
  this->receive_event();
  
  // check for stamp
  if (this->node->controller->stamp_name != "") {
    pugi::xml_node stamp_node = this->data_root.append_child(this->node->controller->stamp_name.c_str());
    this->record_node_data(stamp_node);
  }

  node->tick();
  this->mpi_server->tick();

  // Run the MPI server if the main control flow is in active
  if (!this->node->controller->is_backend_active() 
      && !this->node->mover->is_active() 
      && this->mpi_server->is_active()
      && this->node->controller->decoder->state == DECODER_WAIT4MPI
     ) 
  {

    // ============================= Handle as MPI_SEND / MPI_INTRP_REQ =============================
    // If acknowledgement is not received, send ping
    if (this->mpi_server->state == MPI_SEND_GENPAYLOAD || this->mpi_server->state == MPI_SEND_WAIT4ACK) {
      if (this->acknowledgement_received && this->receiving_ack_from == this->mpi_server->des_network_id) {
        this->mpi_server->acknowledgement_received = true;
      }
      else {
      	Node_Event * ping;
      	if (this->mpi_server->interupt_request == true) {
      		ping = new Node_Event(INTRP_PING, this->network_id, this->mpi_server->des_network_id);
      	}
      	else {
      		ping = new Node_Event(PING, this->network_id, this->mpi_server->des_network_id);
      	}
        //cout << "Sender sending ping over link " << this->link_control << endl;
        // optionaly send ping once every duration (instead of potentially sending every cycle, polluting the NoC)
        if (this->ping_counter == 0) {
      		this->mpi_server->write_to_node("Send_Event", "PING " + to_string(this->mpi_server->des_network_id));
      		this->mpi_server->send_request(ping); // send a ping every cycle until acknowledgement is received
      		this->ping_counter = this->ping_skip_duration;
        }
        else {
        	this->ping_counter -= 1;
        }
      }
    }
    else if (this->mpi_server->state == MPI_SEND_SHUTDOWN) {
      this->acknowledgement_received = false;
      this->ping_received = false;
      this->ping_counter = 0;
      this->mpi_server->shutdown();
      this->node->controller->decoder->state = DECODER_READY;
    }

    // ============================= Handle as MPI_RECV =============================
    if (this->mpi_server->state == MPI_RECV_WAIT4PING) {
      if (this->ping_received && this->receiving_ping_from == this->mpi_server->src_network_id) {
        // Check if the ping is from the correct direction, send ack if it is correct
          Node_Event * ack = new Node_Event(ACK, this->network_id, this->mpi_server->src_network_id);
          //cout << "Receiver sending ack" <<endl;
          this->mpi_server->write_to_node("Send_Event", "ACK " + to_string(this->mpi_server->src_network_id));
          this->mpi_server->ping_received = true;
          this->mpi_server->send_request(ack);
      }
    }
    
    // ============================= Handle as MPI_INTRP_SERVE =============================
    if (this->mpi_server->state == MPI_INTRP_WAIT4PING) {
      // accept ping from anyone, since we are being interupted
      if (this->interupt_ping_received) {
        // Check if the ping is from the correct direction, send ack if it is correct
          Node_Event * ack = new Node_Event(ACK, this->network_id, this->receiving_ping_from);
          this->mpi_server->write_to_node("Send_Event", "ACK " + to_string(this->receiving_ping_from));
          this->mpi_server->interupt_ping_received = true;
          this->mpi_server->send_request(ack);
      }
      // Don't wait for too long, just continue in case there is no in coming interupt
      else {
            if (this->interupt_wait_counter >  0) {
          		this->interupt_wait_counter -= 1;
          		cout << this->interupt_wait_counter  <<endl;
      	    }
      	    else if (this->interupt_wait_counter == 0) {
          		this->mpi_server->shutdown();
          		this->node->controller->decoder->state = DECODER_READY;
          		this->interupt_ping_received = false;
          		this->interupt_wait_counter = this->interupt_serve_wait_duration;
      	    }
      }
    }
    
    // ============================= Handle as MPI_BARRIER =============================
    if (this->mpi_server->state == MPI_BARRIER_WAIT4PING) {
      // if all pings have been received from all peers, retire this state, resume control back to Node frontend
      if (this->num_ping_received == this->num_peers) {
      	this->num_ping_received = 0;
      	for (int64_t i = 0; i < this->num_peers; i+=1) {
      		this->barrier_status[i] = false;
      	}
      	this->mpi_server->state = MPI_IDLE;
      }
      // else, keep sending pings to everyone
      else {
      	for (int64_t peer = 0; peer < this->num_peers; peer+=1) {
      		Node_Event * ping = new Node_Event(PING, this->network_id, peer);
       	  this->mpi_server->write_to_node("Barrier_Event", "PING " + to_string(peer));
        	this->mpi_server->send_request(ping);
      	}
      }
    }

    // See if there are things from MPI receiver to store back before resume control path 
    if(this->store_ready) {
        this->store_back();
    }
  }

  // Kill the sim
  if (node->idle_counter == this->max_idle) {
      primaryComponentOKToEndSim();
      util::report_microkernel_node(node, clk - this->max_idle, this->config_file);
      return true;
  }
  return false;
};
