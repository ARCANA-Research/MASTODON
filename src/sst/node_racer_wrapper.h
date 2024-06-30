#ifndef Node_RACER_WRAPPER_H
#define Node_RACER_WRAPPER_H
#include "../top/node.h"
#include "../top/pum_isa.h"
#include "../top/sst_enable_flag.h"
#include "../middle-racer/cluster.h"
#include "../middle-racer/racer_decoder.h"
#include "../data_movement/data_mover.h"
#include "../bottom/column.h"
#include "../../util/pretty.h"

#include "mpi_server.h"
#include "node_event.h"

#include "pugixml.hpp"
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/sst_config.h>
#include <vector>
#include <string>
#include <queue>
#include <sst/core/interfaces/simpleNetwork.h>
#include "sst/elements/merlin/merlin.h"

using namespace std;
namespace Mastodon {
class Node_Racer_Wrapper : public SST::Component
{
public:
    SST_ELI_REGISTER_COMPONENT(
        Mastodon::Node_Racer_Wrapper,                // Component class
        "mastodon",                          // Component library (for Python/library lookup)
        "node_racer",                               // Component name (for Python/library lookup)
        SST_ELI_ELEMENT_VERSION(1,0,0),      // Version of the component (not related to SST version)
        "Memory Processing Unit w. RACER backend",            // Description
        COMPONENT_CATEGORY_PROCESSOR         // Category
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "config_file",  "Configuration file that describe the Node.", NULL},
        { "binary_file", "Binary file supplied to Node.", NULL},
        { "max_idle", "Number of idle cycles before Node retires.", "1000"},
        { "num_peers", "Number of peer Node in the network.", "1"},
        { "network_id", "Node ID on the network", NULL}
    )

    // no more real ports, everything handled through merlin
    // SST_ELI_DOCUMENT_PORTS(
    //     {"north_port",    "Link to top component",   {" nodeEvent "} },
    //     {"south_port",    "Link to bottom component",{" nodeEvent "} },
    //     {"east_port",     "Link to right component", {" nodeEvent "} },
    //     {"west_port",     "Link to left component",  {" nodeEvent "} }
    // )

    SST_ELI_DOCUMENT_PORTS (
        {"rtr",  "Port that hooks up to router.", { "merlin.RtrEvent", "merlin.credit_event" } }
    )

    SST_ELI_DOCUMENT_STATISTICS( )

    // Node has a single link control that connects to Merlin's port control
    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
        {"networkIF", "Network interface", "SST::Interfaces::SimpleNetwork" }
    )

    Node_Racer_Wrapper(SST::ComponentId_t id, SST::Params& params);
    //SST::ComponentId_t Node_ID;
    SST::Interfaces::SimpleNetwork::Handler<Node_Racer_Wrapper>* send_notify_functor;

    ~Node_Racer_Wrapper();
    void init(unsigned int phase);
    void setup();
    void finish();
 private:
    string config_file;
    string record_element;
    int64_t max_idle;
    int64_t network_id;
    int64_t num_peers;
    int64_t num_ping_received;
    vector <bool> barrier_status;
    int num_vns;
    size_t ping_counter;
    size_t ping_skip_duration;
    size_t interupt_wait_counter;
    size_t interupt_serve_wait_duration;
    size_t last_cycle;
    Node<Cluster, Racer_Decoder> * node;
    SST::Output * sstout;

    pugi::xml_document stat_doc;
    pugi::xml_node stat_root;
    pugi::xml_document data_doc;
    pugi::xml_node data_root;

    MPI_Server * mpi_server;

    // SST::Link* north_link;
    // SST::Link* south_link;
    // SST::Link* east_link;
    // SST::Link* west_link;
    SST::Interfaces::SimpleNetwork * link_control;
    Clock::Handler<Node_Racer_Wrapper>* clock_functor;
    TimeConverter* clock_tc;
    void receive_event();
    virtual bool clockTic(SST::Cycle_t);

    // queue <Node_Event *> north_payload;
    // queue <Node_Event *> south_payload;
    // queue <Node_Event *> east_payload;
    // queue <Node_Event *> west_payload;
    queue <Node_Event *> recv_payload_queue;

    bool acknowledgement_received;
    bool ping_received;
    bool interupt_ping_received;
    bool store_ready;
    size_t receiving_payload_from;
    size_t receiving_ack_from;
    size_t receiving_ping_from;
    size_t store_back_counter;
    size_t mpi_send_store_back_lat;
    size_t mpi_interupt_store_back_lat;
    
    void store_payload(Node_Event * ev);
    void store_back();

    bool send_notify(int vn);
    
    void record_node_data(pugi::xml_node node);
};
}
#endif
