#ifndef SERIAL_MOVER_CPP
#define SERIAL_MOVER_CPP
#include "./serial_mover.h"

template <class Back_End>
Serial_Mover<Back_End>::Serial_Mover(size_t mover_COPY_lat_, size_t mover_SETUP_lat_)
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
void Serial_Mover<Back_End>::get_instruction(PUM_ISA entry)
{
  this->requests.push(entry);
};

template <class Back_End>
void Serial_Mover<Back_End>::get_src_des(size_t mov_src_lane_, size_t mov_des_lane_) {
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
  this->src_des_pairs.push(new_pair);
  this->setup_timer += this->mover_SETUP_lat;
};

template <class Back_End>
void Serial_Mover<Back_End>::erase_src_des() {
  queue <pair <size_t, size_t> > empty1;
  queue <pair <size_t, size_t> > empty2;
  swap(this->src_des_pairs, empty1);
  swap(this->serviced_pairs, empty2);
};

template <class Back_End>
bool Serial_Mover<Back_End>::is_active()
{
  return this->busy;
};

template <class Back_End>
void Serial_Mover<Back_End>::connect_lanes(vector <Back_End *> lanes_)
{
  this->lanes = lanes_;
};

template <class Back_End>
void Serial_Mover<Back_End>::service_new_request() {

  // INTEGRATION NOTICE: Add your micro-arch-specific data movement routine here
    // If there are still pairs of src des to service the same request
    if (!(this->src_des_pairs.empty()))
    {
      size_t src_lane;
      size_t des_lane;
      PUM_ISA request;

      src_lane = src_des_pairs.front().first;
      des_lane = src_des_pairs.front().second;
      this->serviced_pairs.push(this->src_des_pairs.front());
      this->src_des_pairs.pop();
      request = this->requests.front();
    
      // Service request
      if (request.command == "MEM_COPY") {

          /* Old way of doing this requires breaking abstraction (i.e., invoking the RACER-specific pipeline class)
          Bit_Pipeline * src_regfile = this->lanes[src_lane]->pipelines[request.mov_src_regfile_id];
          Bit_Pipeline * des_regfile = this->lanes[des_lane]->pipelines[request.mov_des_regfile_id];
          size_t granularity = src_regfile->granularity;

          for (size_t i = 0; i < granularity; i+=1) {
              Column * des = &(des_regfile->tiles[i]->columns[request.mov_des_reg_addr]);
              Column * src = &(src_regfile->tiles[i]->columns[request.mov_src_reg_addr]);

              des->copy(src);
          }
          */

          
          for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
              Column * src = this->lanes[src_lane]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
              Column * des = this->lanes[des_lane]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i);
              des->copy(src);
          }
          

          this->busy = true;
          this->request_timer = this->mover_COPY_lat;

          this->write_to_node("Lane_Addr", "From lane " + to_string(src_lane) + " to " + to_string(des_lane));
          this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));
      }
      else if (request.command == "MEM_COPY_RSHIFT") {
          for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
		if (i < request.shift_amount) {
			continue;
		}
              Column * src = this->lanes[src_lane]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
              Column * des = this->lanes[des_lane]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i - request.shift_amount);
              des->copy(src);
          }
          

          this->busy = true;
          this->request_timer = this->mover_COPY_lat;

          this->write_to_node("Lane_Addr", "From lane " + to_string(src_lane) + " to " + to_string(des_lane));
          this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));
      }
      else if (request.command == "MEM_BCAST") {
        for (size_t i = 0; i < this->lanes[0]->granularity; i+=1) {
          Column * src = this->lanes[src_lane]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, i);
          Column * des = this->lanes[des_lane]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, i);

          char elem_val = src->raw[request.mov_src_elem_idx];
          string broadcasted_val(this->lanes[0]->num_row, elem_val);
          des->write(broadcasted_val);
        }

        this->write_to_node("Lane_Addr", "From lane " + to_string(src_lane) + " to " + to_string(des_lane));
        this->write_to_node("Request", request.raw.substr(0, request.raw.length() - 1));

        this->busy = true;
        this->request_timer = this->mover_COPY_lat;
      }
      else {
          cout << "ERROR: MOVER: request type not supported: " << request.raw << endl;
          exit(-1);
       }
    }
    // if there are no pair left, repopulate the pair list, then service new request
    else {
      this->requests.pop();
      swap(this->serviced_pairs, this->src_des_pairs);
      if (this->requests.size() != 0) {
          this->service_new_request();
      }
      else {
          this->busy = false;
      }
    }
};

template <class Back_End>
void Serial_Mover<Back_End>::tick()
{
  if (this->busy == false && this->setup_timer > 0) {
    this->write_to_node("Setup_Timer", to_string(this->setup_timer));
    this->setup_timer -= 1;
    if (this->setup_timer == 0) {
      this->busy = false;
    }
  }
  // this statement is used to initiate the first request
  else if (this->busy == false && !(this->requests.empty()) && this->setup_timer == 0) {
    if (this->show) {cout << "Intra-Mover: starting new request sequence" << endl;}
    this->service_new_request();
  }

  if (this->busy) {

    this->request_timer -= 1;
    this->write_to_node("Request_Timer", to_string(this->request_timer));

    if (this->request_timer == 0 && !(this->requests.empty())) {
      if (this->show) {cout << "Intra-Mover: service new requeust" << endl;}
      this->service_new_request();
    }

    else if (this->request_timer == 0 && (this->requests.empty())) {
      if (this->show) {cout << "Intra-Mover: shutdown" << endl;}
      this->busy = false;
    }
  }

  // if at anytime event timer is less than 0, thats wrong
  if (this->request_timer < 0 || this->setup_timer < 0) {
    cout << "ERROR: Data Mover: event timer is less than 0" << endl;
    exit(-1);
  }
};

#endif
