#ifndef MPI_SERVER_H
#define MPI_SERVER_H

#include <vector>
#include <queue>
#include "../middle-racer/cluster.h"
#include "../top/pum_isa.h"
#include "pugixml.hpp"
#include "node_event.h"
#include <sst/core/link.h>
#include <sst/core/component.h>
#include <sst/core/sst_config.h>
#include <sst/core/componentExtension.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include "sst/elements/merlin/merlin.h"
using namespace std;

namespace Mastodon {
enum mpiServerState {MPI_SEND_WAIT4ACK, //< MPI acting as Sender, send ping and wait for ack
                     MPI_SEND_GENPAYLOAD, //< MPI acting as Sender, generating payload
                     MPI_SEND_PAYLOAD, //< MPI acting as Sender, sending payloads to Receiver
                     MPI_SEND_SHUTDOWN, //< MPI acting as Sender, sending last signal to Receiver
                     MPI_RECV_WAIT4PING,
                     MPI_RECV_PAYLOAD,
                     MPI_BARRIER_WAIT4PING,
                     MPI_INTRP_WAIT4PING,
                     MPI_IDLE
                    };

class MPI_Server : public SST::ComponentExtension
{
public:
	MPI_Server (SST::ComponentId_t id, 
                size_t mpi_send_payload_generation_lat_, 
                size_t mpi_interupt_payload_generation_lat_, 
                size_t payload_size_in_bits_,
                size_t PING_size_in_bits_,
                size_t ACK_size_in_bits_,
                size_t DONE_size_in_bits_,
                size_t network_id_,
                SST::Interfaces::SimpleNetwork * link_control
               );
	void generate_payload(PUM_ISA entry);
	bool is_active();
	void tick();
	void connect_lanes(vector <Cluster *> lanes);
    void start_as_MPI_SEND(size_t des_Node_id, size_t mov_src_lane_, size_t mov_des_lane_);
    void start_as_MPI_RECV(size_t src_Node_id);
    void start_as_MPI_BARRIER();
    void start_as_MPI_INTRP_REQ(size_t des_Node_id);
    void start_as_MPI_INTRP_SERVE();
    void shutdown();
    
    size_t des_network_id;
    size_t src_network_id;

    bool acknowledgement_received;
    bool ping_received;
    bool interupt_ping_received;
    bool interupt_request;
    mpiServerState state;
    bool last_payload_request_received;
    void set_log_node(pugi::xml_node parent_node_);
    void show_log(bool show_);
    void record_log(bool record_);
    void write_to_node(string key, string value);

    void send_request(Node_Event * env);
private:
	vector <Cluster *> lanes;
	queue <Mastodon::Node_Event *> send_payload_queue;
	bool active;
    //SST::Interfaces::SimpleNetwork::Handler<Node_Wrapper>* send_notify_functor;
	SST::Interfaces::SimpleNetwork * link_control;

    size_t payload_size_in_bits;
    size_t PING_size_in_bits;
    size_t ACK_size_in_bits;
    size_t DONE_size_in_bits;

	size_t mpi_send_payload_generation_lat;
	size_t mpi_interupt_payload_generation_lat;
	size_t payload_generation_timer;
    SST::ComponentId_t MPI_server_id;
	size_t mov_src_lane;
	size_t mov_des_lane;
    size_t network_id;

    vector<size_t> fetch_src_data(Cluster * lane, size_t regfile, size_t regaddr);

    //bookeeping
    pugi::xml_node parent_node;
    pugi::xml_node this_node;
    bool show;
    bool record;

};
}
#endif
