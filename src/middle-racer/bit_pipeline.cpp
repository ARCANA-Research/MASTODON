#include "bit_pipeline.h"
/**
* @brief Constructor for the Bit_Pipeline class
*
* The constructor instiate all of the tiles and buffers in a class, then connect them
* in the correct manner (as described in the MICRO'21) paper. The MSB tile is a bit
* special because it can forward a column back to the LSB tile, so that is why there
* is a flag that set a tile to be an MSB tile
*
* @param granularity_ number of tiles in a pipeline
* @param num_row_ number of row in a tile
* @param num_col_ number of col in a tile
* @param num_mask_ number of mask in a tile
* @param num_imm_ number of immediates in a tile
* @param ID_ a unique ID (number) associated with the binary storage
*/
Bit_Pipeline::Bit_Pipeline(size_t granularity_,
                           size_t num_row_,
                           size_t num_col_,
                           size_t num_mask_,
                           size_t num_imm_,
                           size_t ID_) {
  this->granularity = granularity_;
  this->num_col = num_col_;
  this->num_row = num_row_;
  this->num_imm = num_imm_;
  this->num_mask = num_mask_;
  this->ID = ID_;

  this->primitive_op_count.insert({"AND", 0});
  this->primitive_op_count.insert({"NAND", 0});
  this->primitive_op_count.insert({"OR", 0});
  this->primitive_op_count.insert({"NOR", 0});
  this->primitive_op_count.insert({"XOR", 0});
  this->primitive_op_count.insert({"NOT", 0});
  this->primitive_op_count.insert({"COPY", 0});
  this->primitive_op_count.insert({"FA", 0});

  for (size_t i = 0; i < this->granularity; i+= 1) {
    this->buffers.push_back(new Column(0, this->num_row));
  }

  for (size_t i = 0; i < this->granularity; i+=1) {
    // this is the torus connection
    if (i == 0) {
      Tile * tile = new Tile(this->ID + i,
                             this->num_mask,
                             this->num_imm,
                             this->num_col,
                             this->num_row,
                             this->buffers[i],
                             this->buffers[this->granularity-1]);
      this->tiles.push_back(tile);
    }
    else {
      Tile * tile = new Tile(this->ID + i,
                             this->num_mask,
                             this->num_imm,
                             this->num_col,
                             this->num_row,
                             this->buffers[i],
                             this->buffers[i-1]);
      this->tiles.push_back(tile);
    }
  }

  this->tiles[this->granularity - 1]->set_MSB(true);
}

map <string, size_t> Bit_Pipeline::report_primitive_op_count() {
  for (size_t i = 0; i < this->granularity; i+=1) {
    map <string, size_t> temp = this->tiles[i]->report_primitive_op_count();
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
};

void Bit_Pipeline::toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL) {
  for (size_t i = 0; i < this->granularity; i+= 1) 
  {
    	this->tiles[i]->toggle_device_model_sim(device_model_sim, memorisation, cycle_time, volt_MAGIC, volt_ISO_BL);
  }
}

/**
* @brief Set the record flag used to record in an XML file
*/
void Bit_Pipeline::record_log(bool record_){
  this->record = record_;
};

/**
* @brief Set the show flag used to printout to terminal
*/
void Bit_Pipeline::set_log_node(pugi::xml_node parent_node_) {
  this->parent_node = parent_node_;
}

/**
* @brief Write message to xml node
*/
void Bit_Pipeline::write_to_node() {
  if (this->record) {
    std::stringstream ss0;
    ss0 << "Pipeline_" << this->ID;
    this->this_node = parent_node.append_child(ss0.str().c_str());
    for (size_t i = 0; i < this->granularity; i+=1) {
      std::stringstream ss;
      ss << "Tile_" << i;
      pugi::xml_node tile_node = this->this_node.append_child(ss.str().c_str());
      pugi::xml_node primitive_count_node = tile_node.append_child("Primitive_Count");

      primitive_count_node.append_child("NOR").text() = to_string(this->tiles[i]->primitive_op_count["NOR"]).c_str();
      primitive_count_node.append_child("OR").text() = to_string(this->tiles[i]->primitive_op_count["OR"]).c_str();
      primitive_count_node.append_child("NAND").text() = to_string(this->tiles[i]->primitive_op_count["NAND"]).c_str();
      primitive_count_node.append_child("AND").text() = to_string(this->tiles[i]->primitive_op_count["AND"]).c_str();
      primitive_count_node.append_child("XOR").text() = to_string(this->tiles[i]->primitive_op_count["XOR"]).c_str();
      primitive_count_node.append_child("NOT").text() = to_string(this->tiles[i]->primitive_op_count["NOT"]).c_str();
      primitive_count_node.append_child("FA").text() = to_string(this->tiles[i]->primitive_op_count["FA"]).c_str();


      // for (size_t j = 0; j < this->num_col; j+=1) {
      //   std::stringstream ss1;
      //   ss1 << "Column_" << j;
      //   pugi::xml_node col_node = tile_node.append_child(ss1.str().c_str());
      //   pugi::xml_node read_node = col_node.append_child("Input");
      //   pugi::xml_node write_node = col_node.append_child("Output");
      //   read_node.append_child("NOR").text() = to_string(this->tiles[i]->columns[j].read_op_count["NOR"]).c_str();
      //   read_node.append_child("OR").text() = to_string(this->tiles[i]->columns[j].read_op_count["OR"]).c_str();
      //   read_node.append_child("NAND").text() = to_string(this->tiles[i]->columns[j].read_op_count["NAND"]).c_str();
      //   read_node.append_child("AND").text() = to_string(this->tiles[i]->columns[j].read_op_count["AND"]).c_str();
      //   read_node.append_child("XOR").text() = to_string(this->tiles[i]->columns[j].read_op_count["XOR"]).c_str();
      //   read_node.append_child("NOT").text() = to_string(this->tiles[i]->columns[j].read_op_count["NOT"]).c_str();
      //   write_node.append_child("NOR").text() = to_string(this->tiles[i]->columns[j].write_op_count["NOR"]).c_str();
      //   write_node.append_child("OR").text() = to_string(this->tiles[i]->columns[j].write_op_count["OR"]).c_str();
      //   write_node.append_child("NAND").text() = to_string(this->tiles[i]->columns[j].write_op_count["NAND"]).c_str();
      //   write_node.append_child("AND").text() = to_string(this->tiles[i]->columns[j].write_op_count["AND"]).c_str();
      //   write_node.append_child("XOR").text() = to_string(this->tiles[i]->columns[j].write_op_count["XOR"]).c_str();
      //   write_node.append_child("NOT").text() = to_string(this->tiles[i]->columns[j].write_op_count["NOT"]).c_str();
      //   write_node.append_child("FA").text() = to_string(this->tiles[i]->columns[j].write_op_count["FA"]).c_str();
      // }
      // for (size_t j = 0; j < this->num_imm; j+=1) {
      //   std::stringstream ss2;
      //   ss2 << "Immediates " << j;
      //   pugi::xml_node imm_node = tile_node.append_child(ss2.str().c_str());
      //   pugi::xml_node read_node = imm_node.append_child("Input");
      //   pugi::xml_node write_node = imm_node.append_child("Output");
      //   read_node.append_child("NOR").text() = to_string(this->tiles[i]->immediates[j].read_op_count["NOR"]).c_str();
      //   read_node.append_child("OR").text() = to_string(this->tiles[i]->immediates[j].read_op_count["OR"]).c_str();
      //   read_node.append_child("NAND").text() = to_string(this->tiles[i]->immediates[j].read_op_count["NAND"]).c_str();
      //   read_node.append_child("AND").text() = to_string(this->tiles[i]->immediates[j].read_op_count["AND"]).c_str();
      //   read_node.append_child("XOR").text() = to_string(this->tiles[i]->immediates[j].read_op_count["XOR"]).c_str();
      //   read_node.append_child("NOT").text() = to_string(this->tiles[i]->immediates[j].read_op_count["NOT"]).c_str();
      //   write_node.append_child("NOR").text() = to_string(this->tiles[i]->immediates[j].write_op_count["NOR"]).c_str();
      //   write_node.append_child("OR").text() = to_string(this->tiles[i]->immediates[j].write_op_count["OR"]).c_str();
      //   write_node.append_child("NAND").text() = to_string(this->tiles[i]->immediates[j].write_op_count["NAND"]).c_str();
      //   write_node.append_child("AND").text() = to_string(this->tiles[i]->immediates[j].write_op_count["AND"]).c_str();
      //   write_node.append_child("XOR").text() = to_string(this->tiles[i]->immediates[j].write_op_count["XOR"]).c_str();
      //   write_node.append_child("NOT").text() = to_string(this->tiles[i]->immediates[j].write_op_count["NOT"]).c_str();
      //   write_node.append_child("FA").text() = to_string(this->tiles[i]->immediates[j].write_op_count["FA"]).c_str();
      // }
    }
  }
}

/**
* @brief Populate a pipeline with data
*
* @param data a 3D data object that holds the values to be populated in the pipelin
* The first dimension is the col position, the second dimension is the row dimension,
* and the third dimension is the bit position along the pipeline.
*/
void Bit_Pipeline::write_data(vector < vector <string> > data) {
  for (size_t t = 0; t < this->granularity; t+=1) {
    vector <string> tile_slice;
    for (size_t c = 0; c < this->num_col; c+=1) {
      string column = "";
      for (size_t r = 0; r < this->num_row; r+=1) {
        column += data[c][r][t];
      }
      tile_slice.push_back(column);
    }
    this->tiles[t]->write_data(tile_slice);
  }
}

/**
* @brief Display the data of the pipeline for debugging
*/
void Bit_Pipeline::display_data() {
  cout << "<Addressable Registers> | <Intermediate Registers> | <Masks> | <Right Buffer>" << endl;
  for (size_t i = 0; i < this->granularity; i+=1) {
    cout << "Tile " << i << endl;
    this->tiles[i]->display_data();
  }
}

/**
* @brief Get a byte out of the pipeline.
*
* @param row the row position
* @param col  the col position
* @param bg which bytegroup should be returned
*
* @return res the bytegroup required
*/
uint8_t Bit_Pipeline::get_byte(size_t row, size_t col, size_t bg) {
  uint8_t res = 0;
  int cnt = 1;

  for (size_t i = 0; i < 8; i++) {
    res += this->tiles[bg * 8 + i]->get_bit(row, col) * cnt;
    cnt *= 2;
  }
  return res;
}

/**
* @brief send a micro-op to a specific tile
*
* This function is called by the Uop_Queue class, sequentially from the LSB
* tile to the MSB tile. The micro-op queue should know which micro-op goes to which
* tile in every single cycle
*
* @param tile_idx the index of the targeted tile in the pipeline
* @param uop the micro-op to be sent to the targeted tile
*
*/
void Bit_Pipeline::execute(size_t tile_idx, Racer_Uop uop) {
  this->tiles[tile_idx]->execute(uop);
}
