#ifndef Node_EVENT_H
#define Node_EVENT_H
#include <string>
#include <sst/core/event.h>
#include <vector>
#include "../top/pum_isa.h"
using namespace std;
namespace Mastodon {
enum EventType {SEND, DONE, ACK, PING, INTRP, INTRP_PING};
class Node_Event : public SST::Event{
private:
    Node_Event() {} // For serialization
public:
    EventType type;
    vector<size_t> src_data;
    size_t des_network_id;
    size_t src_network_id;
    size_t des_lane;
    size_t des_regfile;
    size_t des_regaddr;
    PUM_ISA interupt_instruction;

    Node_Event(EventType type_, size_t src_network_id, size_t des_network_id) {
        this->type = type_;
        this->src_network_id = src_network_id;
        this->des_network_id = des_network_id;
    };

    Node_Event(EventType type_, size_t src_network_id, size_t des_network_id, vector<size_t> src_data_, size_t des_lane_, size_t des_regfile_, size_t des_regaddr_) {
        this->type = type_;
        this->src_network_id = src_network_id;
        this->des_network_id = des_network_id;
        this->des_lane = des_lane_;
        this->des_regfile = des_regfile_;
        this->des_regaddr = des_regaddr_;
        this->src_data = src_data_;
    };
    
    Node_Event(EventType type_, size_t src_network_id, size_t des_network_id, PUM_ISA entry) {
        this->type = type_;
        this->src_network_id = src_network_id;
        this->des_network_id = des_network_id;
        this->interupt_instruction = entry;
    };

    void serialize_order(SST::Core::Serialization::serializer &ser)  override {
        Event::serialize_order(ser);
        ser & type;
        ser & des_network_id;
        ser & src_network_id;
        ser & src_data;

        ser & des_lane;
        ser & des_regfile;
        ser & des_regaddr;
    }
    ImplementSerializable(Mastodon::Node_Event);
};
}
#endif
