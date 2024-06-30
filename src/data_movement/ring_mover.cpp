#ifndef RING_MOVER_CPP
#define RING_MOVER_CPP
#include "./ring_mover.h"

template <class Back_End>
Ring_Mover<Back_End>::Ring_Mover(size_t mover_COPY_lat_, size_t mover_SETUP_lat_)
             :Data_Mover<Back_End>(mover_COPY_lat_, mover_SETUP_lat_)
{
  this->busy = false;
  this->show = false;
  this->mover_COPY_lat = mover_COPY_lat_;
  this->mover_SETUP_lat = mover_SETUP_lat_;

  for (int i = 0; i < NUM_NODE; i+=1) {
    this->node_busy.push_back(false);
  }
};

// first path is always shorter or equal to second path
template <class Back_End>
pair <vector <size_t>, vector <size_t> > Ring_Mover<Back_End>::find_path(size_t src, size_t des){
  size_t smaller;
  size_t bigger;
  pair <vector <size_t>, vector <size_t> > paths;
  if (src == des) {
    vector <size_t> path;
    path.push_back(src);
    paths.first = path;
    paths.second = path;
    return paths;
  }

  if (src > des) {
    smaller = des;
    bigger = src;
  }
  else {
    smaller = src;
    bigger = des;
  }

  //find first path
  vector <size_t> pathA;
  for (size_t node = smaller; node <= bigger; node+=1) {
    pathA.push_back(node);
  }

  // find second path
  vector <size_t> pathB;
  for (size_t node = smaller; node >= 0; node-=1) {
    pathB.push_back(node);
  }
  for (size_t node = NUM_NODE - 1; node >= bigger; node -=1) {
    pathB.push_back(node);
  }

  if (pathA.size() <= pathB.size()) {
    paths.first = pathA;
    paths.second = pathB;
  }
  else {
    paths.first = pathB;
    paths.second = pathA;
  }

  return paths;
};

// true means path can be used
template <class Back_End>
bool Ring_Mover<Back_End>::check_path(vector <size_t> path) {
  for (unsigned int i = 0; i < path.size(); i+=1) {
    if (this->node_busy[path[i]]) {
      return false;
    }
  }
  return true;
};

template <class Back_End>
void Ring_Mover<Back_End>::get_instruction(PUM_ISA entry)
{
  this->requests.push_back(entry);
};

template <class Back_End>
void Ring_Mover<Back_End>::get_src_des(size_t mov_src_lane_, size_t mov_des_lane_) {
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
};

template <class Back_End>
void Ring_Mover<Back_End>::erase_src_des() {
  vector <pair <size_t, size_t> > empty1;
  vector <size_t> empty2;
  vector <vector <size_t> > empty3;
  vector <PUM_ISA> empty4;
  swap(this->src_des_pairs, empty1);
  swap(this->service_timers, empty2);
  swap(this->service_paths, empty3);
  swap(this->requests, empty4);
  for (unsigned int i = 0; i < this->node_busy.size(); i+=1) {
    this->node_busy[i] = false;
  }
};

template <class Back_End>
bool Ring_Mover<Back_End>::is_active()
{
  return this->busy;
};

template <class Back_End>
void Ring_Mover<Back_End>::connect_lanes(vector <Back_End *> lanes_)
{
  this->lanes = lanes_;
};

template <class Back_End>
void Ring_Mover<Back_End>::service_new_request() {

  // INTEGRATION NOTICE: Add your micro-arch-specific data movement routine here
  // RACER backend
    vector <size_t> src_lanes;
    vector <size_t> des_lanes;

    // Queue up as many new paths as possible
    // Check pathA (the shorter one, for all pairs first)
    for (unsigned int i = 0; i < this->src_des_pairs.size(); i+=1) {
      size_t tmp_src = this->src_des_pairs[i].first;
      size_t tmp_des = this->src_des_pairs[i].second;
      pair <vector <size_t>, vector <size_t> > paths = find_path(tmp_src, tmp_des);
      vector <size_t> path = paths.first;
      if (check_path(path)) {
        src_lanes.push_back(tmp_src);
        des_lanes.push_back(tmp_des);
        size_t delay_multiplier = (size_t) path.size();
        this->service_paths.push_back(path);
        this->service_timers.push_back(this->mover_SETUP_lat + delay_multiplier * this->requests.size());
        this->src_des_pairs.erase(this->src_des_pairs.begin() + i);
        this->busy = true;
      }
    }

    // Check pathB (the longer one, for all pairs first)
    for (unsigned int i = 0; i < this->src_des_pairs.size(); i+=1) {
      size_t tmp_src = this->src_des_pairs[i].first;
      size_t tmp_des = this->src_des_pairs[i].second;
      pair <vector <size_t>, vector <size_t> > paths = find_path(tmp_src, tmp_des);
      vector <size_t> path = paths.second;
      if (check_path(path)) {
        src_lanes.push_back(tmp_src);
        des_lanes.push_back(tmp_des);
        size_t delay_multiplier = (size_t) path.size();
        this->service_paths.push_back(path);
        this->service_timers.push_back(this->mover_SETUP_lat + delay_multiplier * this->requests.size());
        this->src_des_pairs.erase(this->src_des_pairs.begin() + i);
        this->busy = true;
      }
    }

    // Service request
    for (unsigned int i = 0; i < this->requests.size(); i+=1) {
      PUM_ISA request = this->requests[i];
      for (unsigned int j = 0; j < src_lanes.size(); j+=1) {
        if (request.command == "MEM_COPY") {
          for (size_t k = 0; k < this->lanes[0]->granularity; k+=1) {
            Column * src = this->lanes[src_lanes[j]]->fetch_regular_register(request.mov_src_regfile_id, request.mov_src_reg_addr, k);
            Column * des = this->lanes[des_lanes[j]]->fetch_regular_register(request.mov_des_regfile_id, request.mov_des_reg_addr, k);
            des->copy(src);
          }
        }
        else {
          cout << "ERROR: MOVER: request type not supported: " << request.raw << endl;
          exit(-1);
        }
      }
    }
};

template <class Back_End>
void Ring_Mover<Back_End>::tick()
{
  // initialize the first request
  if ((this->service_timers.empty()) && !(this->src_des_pairs.empty())) {
    this->service_new_request();
  }

  if (!this->service_timers.empty()) {
    for (unsigned int i = 0; i < this->service_timers.size(); i+=1) {
      this->service_timers[i] -= 1;
      // if one of the timer is 0, then time to create a new path
      if (this->service_timers[i] < 0) {
        this->service_timers.erase(this->service_timers.begin() + i);
        for (unsigned int j = 0; j < this->service_paths[i].size(); j+=1) {
          this->node_busy[this->service_paths[i][j]] = false;
        }
        this->service_new_request();
      }
    }
  }

  // if no timer exist, then mover is not busy
  if (this->service_timers.empty()) {
    this->busy = false;
  }
};

#endif
