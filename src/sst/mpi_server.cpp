#include "mpi_server.h"
using namespace SST;
using namespace Mastodon;
MPI_Server::MPI_Server (SST::ComponentId_t id, 
		                size_t mpi_send_payload_generation_lat_, 
		                size_t mpi_interupt_payload_generation_lat_, 
		                size_t payload_size_in_bits_,
		                size_t PING_size_in_bits_,
		                size_t ACK_size_in_bits_,
		                size_t DONE_size_in_bits_,
		                size_t network_id_,
		                SST::Interfaces::SimpleNetwork * link_control_
		                //SST::Interfaces::SimpleNetwork::Handler<Node_Wrapper>* send_notify_functor_
						) : ComponentExtension(id)
{
	this->mpi_send_payload_generation_lat = mpi_send_payload_generation_lat_;
	this->mpi_interupt_payload_generation_lat = mpi_interupt_payload_generation_lat_;
	this->link_control = link_control_;
	this->show = false;
	this->record = false;
	this->network_id = network_id_;
    //this->send_notify_functor = send_notify_functor_;
	this->payload_size_in_bits = payload_size_in_bits_;
	this->PING_size_in_bits = PING_size_in_bits_;
	this->ACK_size_in_bits = ACK_size_in_bits_;
	this->DONE_size_in_bits = DONE_size_in_bits_;
    this->shutdown();
};

vector <size_t> MPI_Server::fetch_src_data(Cluster * lane, size_t regfile, size_t regaddr) {
    size_t num_row = lane->num_row;
    size_t num_bg = lane->granularity / 8;
    vector <size_t> column;

      for (size_t r = 0; r < num_row; r += 1) {
        size_t value = 0;
        for (int b = num_bg - 1; b >= 0; b -= 1) {
          size_t portion = lane->get_byte(regfile, b, r, regaddr);
          portion = portion << (8 * b);
          value += portion;
        }
        column.push_back(value);
      }
      return column;
};

void MPI_Server::generate_payload(PUM_ISA entry) {
	// if it is interupt, then just create the right event and put it in the queue
	if (this->interupt_request == true) {
		this->payload_generation_timer += this->mpi_interupt_payload_generation_lat;
		Mastodon::Node_Event * payload = new Mastodon::Node_Event(INTRP,
									this->network_id,
									this->des_network_id,
									entry);
		this->send_payload_queue.push(payload);
	    	this->state = MPI_SEND_GENPAYLOAD;

	    	//bookeeping
	    	this->write_to_node("Send_Event", "Dec." + entry.raw);
	}
	// else, fetch the data source and generate the event
	else {
		this->payload_generation_timer += this->mpi_send_payload_generation_lat;
		size_t src_regfile = entry.mov_src_regfile_id;
		size_t src_regaddr = entry.mov_src_reg_addr;
		size_t des_regfile = entry.mov_des_regfile_id;
		size_t des_regaddr = entry.mov_des_reg_addr;
		vector <size_t> src_data = fetch_src_data(this->lanes[this->mov_src_lane], src_regfile, src_regaddr);
		Mastodon::Node_Event * payload = new Mastodon::Node_Event(SEND, 
									 this->network_id, 
									 this->des_network_id, 
									 src_data, 
									 this->mov_des_lane, 
									 des_regfile, 
									 des_regaddr);
		this->send_payload_queue.push(payload);
	    	this->state = MPI_SEND_GENPAYLOAD;

	    	//bookeeping
	    	this->write_to_node("Send_Event", "Dec." + entry.raw);
    	}
};

bool MPI_Server::is_active(){
	return this->active;
};

void MPI_Server::connect_lanes(vector <Cluster *> lanes_){
	this->lanes = lanes_;
};

void MPI_Server::set_log_node(pugi::xml_node parent_node_) {
	this->parent_node = parent_node_;
	this->this_node = this->parent_node.append_child(("MPI_Server"));
}

void MPI_Server::write_to_node(string key, string value) {
if (this->record == true) {
    this->this_node.append_child(key.c_str()).text() = value.c_str();
  }
}

void MPI_Server::record_log(bool record_) {
	this->record = record_;
}
void MPI_Server::show_log(bool show_) {
	this->show = show_;
}

// Called by decoder
void MPI_Server::start_as_MPI_SEND(size_t des_network_id, size_t mov_src_lane_, size_t mov_des_lane_){
	this->state = MPI_SEND_WAIT4ACK;
    	this->mov_src_lane = mov_src_lane_;
	this->mov_des_lane = mov_des_lane_;
	this->active = true;
	this->des_network_id = des_network_id;

	//bookeeping
	this->write_to_node("Receive_Event", "Dec. SEND");
};

// Called by decoder
void MPI_Server::start_as_MPI_RECV(size_t  src_network_id){
	this->active = true;
	this->state = MPI_RECV_WAIT4PING;
	this->src_network_id = src_network_id;

	//bookeeping
	this->write_to_node("Receive_Event", "Dec. RECV");
};

// Called by decoder
void MPI_Server::start_as_MPI_BARRIER(){
	this->active = true;
	this->state = MPI_BARRIER_WAIT4PING;
};

// Called by decoder
void MPI_Server::start_as_MPI_INTRP_REQ(size_t des_network_id) {
	this->active = true;
	this->interupt_request = true;
	this->state = MPI_SEND_WAIT4ACK;
	this->des_network_id = des_network_id;
}

// Called by decoder
void MPI_Server::start_as_MPI_INTRP_SERVE() {
	this->active = true;
	this->state = MPI_INTRP_WAIT4PING;
}

void MPI_Server::shutdown() {
	// general communication
	this->acknowledgement_received = false;
	this->ping_received = false;
	this->interupt_ping_received = false;
	this->active = false;
    	this->state = MPI_IDLE;
    	
    	// MPI_SEND related
    	this->last_payload_request_received = false;
	this->payload_generation_timer = 0;
	
	// interupt related
	this->interupt_request = false;

	if (this->send_payload_queue.size() != 0) {
		printf("ERROR: MPI Server: premature shutdown");
	}
};

void MPI_Server::send_request(Node_Event * evn) {
	int packet_size;
	if (evn->type == SEND || evn->type == INTRP) {
		packet_size = (int) this->payload_size_in_bits;
	}
	else if (evn->type == PING || evn->type == INTRP_PING) {
		packet_size = (int) this->PING_size_in_bits;
	}
	else if (evn->type == ACK) {
		packet_size = (int) this->ACK_size_in_bits;
	}
	else if (evn->type == DONE) {
		packet_size = (int) this->DONE_size_in_bits;
	}

	if (this->link_control->spaceToSend(0, packet_size)) {
		SST::Interfaces::SimpleNetwork::Request * req = new SST::Interfaces::SimpleNetwork::Request();
		req->head = true;
		req->tail = true;
		req->dest = (long int) evn->des_network_id;
		req->src = (long int) evn->src_network_id;
		req->vn = 0;
		req->size_in_bits = packet_size;
		req->givePayload(evn);
		bool sent = this->link_control->send(req,0);
		assert(sent);
	}
	//else {
	//	this->link_control->setNotifyOnSend(this->send_notify_functor);
	//}
	//return true;
}

void MPI_Server::tick(){
    if (this->active) {
       switch (this->state) {
       		// only the Node frontend can invoke the server and break it from this IDLE loop
	        case (MPI_IDLE): {
	            this->state = MPI_IDLE;
                if(this->show) {cout << "MPI Idle" << endl;}
	            break;
	        }
	        case (MPI_SEND_GENPAYLOAD): {
	            if (this->last_payload_request_received && this->payload_generation_timer == 0) {
	                if (this->acknowledgement_received) {
	                    this->state = MPI_SEND_PAYLOAD;
	                    //bookeeping, not important
	                    this->write_to_node("State", "Send_Payload");
                            if(this->show) {cout << "MPI Send Payloads" << endl;}
	                }
	                else {
	                    this->state = MPI_SEND_WAIT4ACK;

	                    //bookeeping, not important
	                    this->write_to_node("State", "Wait_ACK");
                        if(this->show) {cout << "MPI Wait for ACK" << endl;}
	                }
	            }
	            else {
	                this->state = MPI_SEND_GENPAYLOAD;
	                if (this->payload_generation_timer > 0) {
	                    this->payload_generation_timer -= 1;

	                    //bookeeping, not important
	                    this->write_to_node("State", "Gen_Payload" );
                        if(this->show) {cout << "MPI Generating Payload" << endl;}
	                }
	            }
	            break;
	        }
	        case (MPI_SEND_PAYLOAD): {
	              if (this->send_payload_queue.size() != 0) {
	              	this->send_request(this->send_payload_queue.front());
	              	this->send_payload_queue.pop();
	              	this->state = MPI_SEND_PAYLOAD;

	              	//bookeeping, not important
	                this->write_to_node("State", "Send_Payload" );
	                this->write_to_node("Send_Event", "Payload " + to_string(this->des_network_id));
                    	if(this->show) {cout << "MPI Send Payloads" << endl;}
	              }
	              else {
	              	this->state = MPI_SEND_SHUTDOWN;
	              	Node_Event * done = new Node_Event(DONE, this->network_id, this->des_network_id);
	              	this->send_request(done);

	              	//bookeeping, not important
	                this->write_to_node("State", "Shutdown" );
	                this->write_to_node("Send_Event", "Shutdown " + to_string(this->des_network_id));
                    if(this->show) {cout << "MPI Shutdown" << endl;}
	              }
	            break;
	        }
	        case (MPI_SEND_WAIT4ACK): {
	        	if (this->acknowledgement_received) {
	        		this->state = MPI_SEND_PAYLOAD;
	        	}
	        	else {
	        		this->state = MPI_SEND_WAIT4ACK;
	        	}

	        	//bookeeping, not important
	            this->write_to_node("State", "Wait_ACK" );
                if(this->show) {cout << "MPI Wait for ACK" << endl;}
	        	break;
	        }
	        
	        case (MPI_SEND_SHUTDOWN): {
	    		this->state = MPI_SEND_SHUTDOWN;
	    		break;
	    	}
	    	
	    	case (MPI_RECV_WAIT4PING): {
	    		if (this->ping_received) {
	    			this->state = MPI_RECV_PAYLOAD;

	    			//bookeeping, not important
	            	this->write_to_node("State", "Recv_Payload" );
	        		if(this->show) {cout << "MPI Receive Payloads" << endl;}
	    		}
	    		else {
	    			this->state = MPI_RECV_WAIT4PING;

	    			//bookeeping, not important
	            	this->write_to_node("State", "Wait_RECV_PING" );
	        		if(this->show) {cout << "MPI Wait for PING" << endl;}
	    		}
	        	break;
	        }
	        
	        case (MPI_INTRP_WAIT4PING): {
	    		if (this->interupt_ping_received) {
	    			this->state = MPI_RECV_PAYLOAD;

	    			//bookeeping, not important
	            	this->write_to_node("State", "Recv_Payload" );
	        		if(this->show) {cout << "MPI Receive Payloads" << endl;}
	    		}
	    		else {
	    			this->state = MPI_INTRP_WAIT4PING;

	    			//bookeeping, not important
	            	this->write_to_node("State", "Wait_INTRP_PING" );
	        		if(this->show) {cout << "MPI Wait for PING" << endl;}
	    		}
	        	break;
	        }

	    	case (MPI_RECV_PAYLOAD): {
	    		this->state = MPI_RECV_PAYLOAD; // shutdown can break this loop
	    		//bookeeping, not important
	            	this->write_to_node("State", "Recv_Payload" );
	        	if(this->show) {cout << "MPI Receive Payloads" << endl;}
	        	break;
	    	}
	    	case (MPI_BARRIER_WAIT4PING): {
	    		this->state = MPI_BARRIER_WAIT4PING; // node_wrapper can break this loop
	    		//bookeeping, not important
	            	this->write_to_node("State", "Barrier_Wait" );
	        	if(this->show) {cout << "Waiting for Barrier Ping" << endl;}
	        	break;
	    	}
       }
    }
};
