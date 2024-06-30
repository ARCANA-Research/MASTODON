#ifndef P2P_MOVER_CPP
#define P2P_MOVER_CPP
#include "./p2p_mover.h"

template <class Back_End>
P2P_Mover<Back_End>::P2P_Mover(size_t mover_COPY_lat_, size_t mover_SETUP_lat_)
             :Data_Mover<Back_End>(mover_COPY_lat_, mover_SETUP_lat_)
{
  this->busy = false;
  this->show = false;
  this->request_timer = 0;
  this->setup_timer = 0;
  this->mover_COPY_lat = mover_COPY_lat_;
  this->mover_SETUP_lat = mover_SETUP_lat_;
};

template <class Back_End>
void P2P_Mover<Back_End>::get_instruction(PUM_ISA entry)
{
  this->requests.push(entry);
  this->busy = true;
};

template <class Back_End>
void P2P_Mover<Back_End>::get_src_des(size_t mov_src_lane_, size_t mov_des_lane_) {
  // Check if lane is already busy
  // for (unsigned int i = 0; i < this->src_des_pairs.size(); i+=1) {
  //   if (mov_src_lane_ == this->src_des_pairs[i].first
  //       || mov_src_lane_ == this->src_des_pairs[i].second
  //       || mov_des_lane_ == this->src_des_pairs[i].first
  //       || mov_des_lane_ == this->src_des_pairs[i].second) {
  //         cout << "ERROR: Data Mover: lane is already busy" << endl;
  //         exit(-1);
  //       }
  // }
  pair <size_t, size_t> new_pair(mov_src_lane_, mov_des_lane_);
  this->src_des_pairs.push_back(new_pair);
  this->setup_timer += this->mover_SETUP_lat;
};

template <class Back_End>
void P2P_Mover<Back_End>::erase_src_des() {
  vector <pair <size_t, size_t> > empty1;
  swap(this->src_des_pairs, empty1);
};

template <class Back_End>
bool P2P_Mover<Back_End>::is_active()
{
  return this->busy;
};

template <class Back_End>
void P2P_Mover<Back_End>::connect_lanes(vector <Back_End *> lanes_)
{
  this->lanes = lanes_;
};

template <class Back_End>
void P2P_Mover<Back_End>::service_new_request() {

  // INTEGRATION NOTICE: Add your micro-arch-specific data movement routine here
  // RACER backend
    PUM_ISA request = this->requests.front();
    for (unsigned int j = 0; j < this->src_des_pairs.size(); j+=1) {
      // Service request
      if (request.command == "MEM_COPY") {

        /* Old way of doing this requires breaking abstraction (i.e., invoking the RACER-specific pipeline class)
        Bit_Pipeline * src_regfile = this->lanes[this->src_des_pairs[j].first]->pipelines[request.mov_src_regfile_id];
        Bit_Pipeline * des_regfile = this->lanes[this->src_des_pairs[j].second]->pipelines[request.mov_des_regfile_id];
        size_t granularity = src_regfile->granularity;
        for (size_t i = 0; i < granularity; i+=1) {
          Column * des = &(des_regfile->tiles[i]->columns[request.mov_des_reg_addr]);
          Column * src = &(src_regfile->tiles[i]->columns[request.mov_src_reg_addr]);

          des->copy(src);
        }
        */

        
        for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
          Column * src = this->lanes[this->src_des_pairs[j].first]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
          Column * des = this->lanes[this->src_des_pairs[j].second]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i);

          des->copy(src);
        }
        
        

        this->write_to_node("Lane_Addr", "From lane " + to_string(this->src_des_pairs[j].first) + " to " + to_string(this->src_des_pairs[j].second));
        this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));

        this->busy = true;
        this->request_timer = this->mover_COPY_lat;
      }
      
      else if (request.command == "MEM_COPY_RSHIFT") {
        for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
          if (i < request.shift_amount) {
          	continue;
          }
          
          Column * src = this->lanes[this->src_des_pairs[j].first]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
          Column * des = this->lanes[this->src_des_pairs[j].second]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i - request.shift_amount);

          des->copy(src);
        }
        this->write_to_node("Lane_Addr", "From lane " + to_string(this->src_des_pairs[j].first) + " to " + to_string(this->src_des_pairs[j].second));
        this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));

        this->busy = true;
        this->request_timer = this->mover_COPY_lat;
      }
      
      else if (request.command == "MEM_BCAST") {
        for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
          Column * src = this->lanes[this->src_des_pairs[j].first]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
          Column * des = this->lanes[this->src_des_pairs[j].second]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i);

          char elem_val = src->raw[request.mov_src_elem_idx];
          string broadcasted_val(this->lanes[0]->num_row, elem_val);
          des->write(broadcasted_val);
        }

        this->write_to_node("Lane_Addr", "From lane " + to_string(this->src_des_pairs[j].first) + " to " + to_string(this->src_des_pairs[j].second));
        this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));

        this->busy = true;
        this->request_timer = this->mover_COPY_lat;
      }
      else {
        cout << "ERROR: MOVER: request type not supported: " << request.raw << endl;
        exit(-1);
      }
    }
    this->requests.pop();
};

template <class Back_End>
void P2P_Mover<Back_End>::tick()
{
  if (this->setup_timer != 0) {
    this->write_to_node("Setup_Timer", to_string(this->setup_timer));
    this->setup_timer -= 1;
  }
  else if (this->busy) {
    // If it is time to service new request
    if (this->request_timer == 0 && !(this->requests.empty())) {
      this->service_new_request();
      if (this->show) {cout << "Intra-Mover: starting new request sequence" << endl;}
    }
    // If it is time to retire the data mover
    else if (this->request_timer == 0 && (this->requests.empty())) {
      if (this->show) {cout << "Intra-Mover: shutdown" << endl;}
      this->busy = false;
      this->setup_timer = 0;
      this->request_timer = 0;
    }
    // Mover is busy with a request
    else {
      this->request_timer -= 1;
      this->write_to_node("Request_Timer", to_string(this->request_timer));
    }
  }

  // if at anytime event timer is less than 0, thats wrong
  if (this->request_timer < 0 || this->setup_timer < 0) {
    cout << "ERROR: Data Mover: event timer is less than 0" << endl;
    exit(-1);
  }
};

#endif
