#include "tile.h"
/**
* @brief Constructor for the Tile class
*
* The constructor casts several Column objects into different things: group of addressable columns
* intermediate columns, constant columns and mask columns. All are set to 0 except for constant column
* with name "c1" which is set to be all ones indefinitely. These constant columns are useful for computation
* from time to time
*
* @param num_mask_ number of mask columns
* @param num_imm_ number of intermediate columns
* @param num_col_ number of regular columns in a tile
* @param num_row_ number of rows. All columns will have the same number of rows
* @param left_buffer_ pointer to the left buffer, which is also a column. It will be connected by the Bit_Pipeline class
* @param left_buffer_ pointer to the right buffer, which is also a column. It will be connected by the Bit_Pipeline class
*/
Tile::Tile(size_t tile_id_,
           size_t num_mask_,
           size_t num_imm_,
           size_t num_col_,
           size_t num_row_,
           Column * left_buffer_,
           Column * right_buffer_)
{
  this->tile_id = tile_id_;
  this->num_mask = num_mask_;
  this->num_imm = num_imm_;
  this->num_col = num_col_;
  this->num_row = num_row_;
  this->left_buffer = left_buffer_;
  this->right_buffer = right_buffer_;

  this->analog_error_counter = 0;

  this->primitive_op_count.insert({"AND", 0});
  this->primitive_op_count.insert({"NAND", 0});
  this->primitive_op_count.insert({"OR", 0});
  this->primitive_op_count.insert({"NOR", 0});
  this->primitive_op_count.insert({"XOR", 0});
  this->primitive_op_count.insert({"NOT", 0});
  this->primitive_op_count.insert({"COPY", 0});
  this->primitive_op_count.insert({"FA", 0});

  for (size_t i = 0; i < this->num_col; i+=1) {
    Column col (0, this->num_row);
    col.ID = i;
    this->columns.push_back(col);
  }

  for (size_t i = 0; i < this->num_imm; i+=1) {
    Column col (0, this->num_row);
    col.ID = i;
    this->immediates.push_back(col);
  }

  for (size_t i = 0; i < this->num_mask; i+=1) {
    Column col (0, this->num_row);
    col.ID = i;
    this->masks.push_back(col);
  }

  string all_zeros(this->num_row, '0');
  string all_ones(this->num_row, '1');
  

  this->constants.push_back(Column(all_zeros, this->num_row));
  this->constants.push_back(Column(all_ones, this->num_row));
  this->MSB_tile = false;
  this->reset();
}

map <string, size_t> Tile::report_primitive_op_count() {
  // for (size_t i = 0; i < this->num_col; i+=1) {
  //   this->primitive_op_count["AND"] += this->columns[i].write_op_count["AND"];
  //   this->primitive_op_count["NAND"] += this->columns[i].write_op_count["NAND"];
  //   this->primitive_op_count["OR"] += this->columns[i].write_op_count["OR"];
  //   this->primitive_op_count["NOR"] += this->columns[i].write_op_count["NOR"];
  //   this->primitive_op_count["XOR"] += this->columns[i].write_op_count["XOR"];
  //   this->primitive_op_count["NOT"] += this->columns[i].write_op_count["NOT"];
  //   this->primitive_op_count["COPY"] += this->columns[i].write_op_count["COPY"];
  //   this->primitive_op_count["FA"] += this->columns[i].write_op_count["FA"];
  // }

  // for (size_t i = 0; i < this->num_imm; i+=1) {
  //   this->primitive_op_count["AND"] += this->immediates[i].write_op_count["AND"];
  //   this->primitive_op_count["NAND"] += this->immediates[i].write_op_count["NAND"];
  //   this->primitive_op_count["OR"] += this->immediates[i].write_op_count["OR"];
  //   this->primitive_op_count["NOR"] += this->immediates[i].write_op_count["NOR"];
  //   this->primitive_op_count["XOR"] += this->immediates[i].write_op_count["XOR"];
  //   this->primitive_op_count["NOT"] += this->immediates[i].write_op_count["NOT"];
  //   this->primitive_op_count["COPY"] += this->immediates[i].write_op_count["COPY"];
  //   this->primitive_op_count["FA"] += this->immediates[i].write_op_count["FA"];
  // }
  return this->primitive_op_count;
}

std::pair<int, int> Tile::report_analog_error() 
{
  map <string, size_t> op_count = this->report_primitive_op_count();
	cout << "  For bit " << this->tile_id << ", " << this->analog_error_counter << " out of " << op_count["NOR"] << " NOR operations are incorrect" << endl;
  return std::make_pair(this->analog_error_counter, op_count["NOR"]);
}

void Tile::simulate_verilogA_memorisation(std::vector<std::string>& stimuli)
{
  if (stimuli[0] == "WRITE")
  {
    // Update states_binary with the given states
    pugi::xml_document doc_binary;
    pugi::xml_parse_result result_binary = doc_binary.load_file("../../src/device-model-plugins/states_binary.xml");
    pugi::xml_node all_states_binary = doc_binary.child("all_states");
    string tile_name = "tile_" + stimuli[1];
    pugi::xml_node tile_binary = all_states_binary.child(tile_name.c_str());
    string col_name = "col_" + stimuli[2];
    pugi::xml_node col = tile_binary.child(col_name.c_str());
    tile_binary.remove_child(col);
    col = tile_binary.append_child(col_name.c_str());
    col.append_child(pugi::node_pcdata).set_value(stimuli[3].c_str());
    doc_binary.save_file("../../src/device-model-plugins/states_binary.xml");

    // Update states with the given states
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("../../src/device-model-plugins/states.xml");
    pugi::xml_node all_states = doc.first_child();
    tile_name = "tile_" + stimuli[1];
    pugi::xml_node tile = all_states.child(tile_name.c_str());
    for (size_t r = 0; r < this->num_row; r+=1)
    {
      string row_name = "row_" + std::to_string(r);;
      pugi::xml_node row = tile.child(row_name.c_str());
      string row_states = row.text().get();
      tile.remove_child(row);
      row = tile.append_child(row_name.c_str());

      // Split the row states into elements
      std::vector<std::string> elements;
      std::string token;
      std::stringstream tokenStream(row_states);
      while (std::getline(tokenStream, token, ',')) 
        elements.push_back(token);

      elements[std::stoi(stimuli[2])] = stimuli[3][r];

      // Join the elements back into a single string
      std::ostringstream oss;
      for (size_t i = 0; i < elements.size(); ++i) {
          if (i != 0) {
              oss << ',';
          }
          oss << elements[i];
      }
      std::string new_row_states = oss.str();
      row.append_child(pugi::node_pcdata).set_value(new_row_states.c_str());
    }
    doc.save_file("../../src/device-model-plugins/states.xml");
  }
  else
  {
    // Update output state to 1 in states_binary
    pugi::xml_document doc_binary;
    pugi::xml_parse_result result_binary = doc_binary.load_file("../../src/device-model-plugins/states_binary.xml");
    pugi::xml_node all_states_binary = doc_binary.child("all_states");
    string tile_name = "tile_" + stimuli[1];
    pugi::xml_node tile_binary = all_states_binary.child(tile_name.c_str());
    string col_name = "col_" + stimuli[4];
    pugi::xml_node col = tile_binary.child(col_name.c_str());
    tile_binary.remove_child(col);
    col = tile_binary.append_child(col_name.c_str());
    string all_one = "";
    for (size_t i = 0; i < this->num_row; i++) {
      all_one += "1";
    }
    string col_state = all_one;
    col.append_child(pugi::node_pcdata).set_value(col_state.c_str());

    // Update output state to 1 in states
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("../../src/device-model-plugins/states.xml");
    pugi::xml_node all_states = doc.first_child();
    tile_name = "tile_" + stimuli[1];
    pugi::xml_node tile = all_states.child(tile_name.c_str());
    for (size_t r = 0; r < this->num_row; r+=1)
    {
      string row_name = "row_" + std::to_string(r);;
      pugi::xml_node row = tile.child(row_name.c_str());
      string row_states = row.text().get();
      tile.remove_child(row);
      row = tile.append_child(row_name.c_str());

      // Split the row states into elements
      std::vector<std::string> row_states_elements;
      std::string token;
      std::stringstream tokenStream(row_states);
      while (std::getline(tokenStream, token, ',')) 
        row_states_elements.push_back(token);

      row_states_elements[std::stoi(stimuli[4])] = "1";

      // Join the elements back into a single string
      std::ostringstream oss;
      for (size_t i = 0; i < row_states_elements.size(); ++i) {
          if (i != 0) {
              oss << ',';
          }
          oss << row_states_elements[i];
      }
      std::string new_row_states = oss.str();

      row.append_child(pugi::node_pcdata).set_value(new_row_states.c_str());
    }

    // Perform NOR operation
    string output_col_states = "";
    for (size_t r = 0; r < this->num_row; r+=1)
    {
      string row_name = "row_" + std::to_string(r);;
      pugi::xml_node row = tile.child(row_name.c_str());
      string row_states = row.text().get();
      tile.remove_child(row);
      row = tile.append_child(row_name.c_str());

      // Split the row states into elements
      std::vector<std::string> row_states_elements;
      std::string token;
      std::stringstream tokenStream(row_states);
      while (std::getline(tokenStream, token, ',')) 
        row_states_elements.push_back(token);

      string initial_output_state = row_states_elements[std::stoi(stimuli[4])];
      string input_1_state = row_states_elements[std::stoi(stimuli[2])];
      string input_2_state = row_states_elements[std::stoi(stimuli[3])];
      if (input_1_state == "1" || input_1_state == "0")
        input_1_state += ".0";
      if (input_2_state == "1" || input_2_state == "0")
        input_2_state += ".0";
      input_1_state = input_1_state.substr(0, 3);
      input_2_state = input_2_state.substr(0, 3);

      // Count the number of isolation cells that are in LRS
      int no_of_LRS_iso_cell = 0;
      for (size_t c = 0; c < this->num_col + this->num_row; c+=1)
      {
        string col_name = "col_" + to_string(c);
        pugi::xml_node col = tile_binary.child(col_name.c_str());
        string col_states = col.text().get();
        char state = col_states[r];
        if (c != std::stoi(stimuli[2]) || c != std::stoi(stimuli[3]) || c != std::stoi(stimuli[4]))
        {
          if (state == '1')
          {
            no_of_LRS_iso_cell += 1;
          }
        }
      }
      
      pugi::xml_document doc_output;
      pugi::xml_parse_result result_output = doc_output.load_file("../../src/device-model-plugins/memorized_states_device-1.xml");
      string key1 = "output_states";
      string key2 = "voltage_" + stimuli[6] + '_' + stimuli[7];
      string key3 = "input_states_" + input_1_state + "_" + input_2_state;
      string key4 = "no_of_LRS_cells_" + std::to_string(no_of_LRS_iso_cell);
      string key5 = "cycle_time_" + stimuli[5];
      pugi::xml_node node = doc_output.child(key1.c_str()).child(key2.c_str()).child(key3.c_str()).child(key4.c_str()).child(key5.c_str());
  
      string predicted_output_state = node.text().get();
      row_states_elements[std::stoi(stimuli[4])] = predicted_output_state;
      // cout << "==========================" << endl;
      // cout << "Tile ID: " << stimuli[1] << endl;
      // cout << "Input 1 state: " << input_1_state << endl;
      // cout << "Input 1 Address Interpreted: " << std::stoi(stimuli[2]) << endl;
      // cout << "Input 2 state: " << input_2_state << endl;
      // cout << "Input 2 Address Interpreted: " << std::stoi(stimuli[3]) << endl;
      // cout << "# of LRS state: " << no_of_LRS_iso_cell << endl;
      // cout << "Predicted state: " << predicted_output_state << endl;
      // cout << "==========================" << endl;
      // Join the elements back into a single string
      std::ostringstream oss;
      for (size_t i = 0; i < row_states_elements.size(); ++i) {
          if (i != 0) {
              oss << ',';
          }
          oss << row_states_elements[i];
      }
      std::string new_row_states = oss.str();

      row.append_child(pugi::node_pcdata).set_value(new_row_states.c_str());
      float final_output_state_flt = std::stof(predicted_output_state);
      if (final_output_state_flt > 0.5)
       output_col_states += "1";
      else
        output_col_states += "0";
    }
    col_name = "col_" + stimuli[4];
    col = tile_binary.child(col_name.c_str());
    tile_binary.remove_child(col);
    col = tile_binary.append_child(col_name.c_str());
    col.append_child(pugi::node_pcdata).set_value(output_col_states.c_str());
    

    doc_binary.save_file("../../src/device-model-plugins/states_binary.xml");
    doc.save_file("../../src/device-model-plugins/states.xml");
  }
}

void Tile::simulate_verilogA(wchar_t* argv[])
{
	Py_Initialize();
	PyObject *obj = Py_BuildValue("s", "../../src/device-model-plugins/Py_Tile.py");
	FILE *file = _Py_fopen_obj(obj, "r+");
	
	if (argv[1] == L"WRITE" || argv[1] == L"INIT") {
		PySys_SetArgvEx(5, argv, 0);
	}
	else {
		PySys_SetArgvEx(6, argv, 0);
	}
	
	if(file != NULL) {
		PyRun_SimpleFile(file, "../../src/device-model-plugins/Py_Tile.py");
		fclose(file);
	}
}

void Tile::update_states(int tile_id)
{
    pugi::xml_document doc;
    pugi::xml_node declaration = doc.append_child(pugi::node_declaration);
    declaration.append_attribute("version") = "1.0";
    declaration.append_attribute("encoding") = "UTF-8";

    pugi::xml_parse_result result = doc.load_file("../../src/device-model-plugins/states.xml");
    pugi::xml_node all_states = doc.first_child();

    if (all_states.empty())
    {
        pugi::xml_node all_states = doc.append_child("all_states");
        string tile_name = "tile_" + std::to_string(tile_id);
        pugi::xml_node tile = all_states.append_child(tile_name.c_str());

        for (size_t r = 0; r < this->num_row; r+=1)
        {
          string row_name = "row_" + std::to_string(r);
          pugi::xml_node row = tile.append_child(row_name.c_str());
          string row_state = "";
          for (size_t c = 0; c < this->num_col+this->num_imm+2; c+=1)
          {
            if (c == 0)
              row_state = this->columns[c].raw[r];
            else if (c < this->num_col)
              row_state = row_state + ',' + this->columns[c].raw[r];
            else if ((c >= this->num_col) && (c < this->num_col + this->num_imm))
              row_state = row_state + ',' + this->immediates[c - this->num_col].raw[r];
            else if (c == this->num_col + this->num_imm) 
              row_state = row_state + ',' + "0";
            else 
              row_state = row_state + ',' + "1";
          }
          row.append_child(pugi::node_pcdata).set_value(row_state.c_str());
        }
    }
    else
    {
        pugi::xml_node all_states = doc.child("all_states");
        string tile_name = "tile_" + std::to_string(tile_id);
        pugi::xml_node tile = all_states.child(tile_name.c_str());

        if (tile.empty())
        {
            pugi::xml_node tile = all_states.append_child(tile_name.c_str());

            for (size_t r = 0; r < this->num_row; r+=1)
            {
              string row_name = "row_" + std::to_string(r);
              pugi::xml_node row = tile.append_child(row_name.c_str());
              string row_state;
              for (size_t c = 0; c < (this->num_col+this->num_imm+2); c+=1)
              {
                if (c == 0)
                  row_state = this->columns[c].raw[r];
                else if (c < this->num_col)
                  row_state = row_state + ',' + this->columns[c].raw[r];
                else if ((c >= this->num_col) && (c < this->num_col + this->num_imm))
                  row_state = row_state + ',' + this->immediates[c - this->num_col].raw[r];
                else if (c == this->num_col + this->num_imm) 
                  row_state = row_state + ',' + "0";
                else 
                  row_state = row_state + ',' + "1";
              }
              row.append_child(pugi::node_pcdata).set_value(row_state.c_str());
            }
        }
        // else
        // {
        //   for (size_t r = 0; r < this->num_row; r+=1)
        //   {
        //     string row_name = "row_" + std::to_string(r);
        //     pugi::xml_node row = tile.child(row_name.c_str());
        //     string row_states = row.text().get();
        //     tile.remove_child(row);
        //     row = tile.append_child(row_name.c_str());
        //     for (size_t c = 0; c < this->num_col+this->num_imm+2; c+=1)
        //     {
        //         if (c < this->num_col)
        //           row_states = row_states + ',' + this->columns[c].raw[r];
        //         else if ((c >= this->num_col) && (c < this->num_col + this->num_imm))
        //           row_states = row_states + ',' + this->immediates[c - this->num_col].raw[r];
        //         else if (c == this->num_col + this->num_imm) 
        //           row_states = row_states + ',' + "1";
        //         else 
        //           row_states = row_states + ',' + "0";
        //         row.append_child(pugi::node_pcdata).set_value(row_states.c_str());
        //     }
        //   }
        // }
    }
    doc.save_file("../../src/device-model-plugins/states.xml");
}

void Tile::update_states_binary(int tile_id)
{
    pugi::xml_document doc;
    pugi::xml_node declaration = doc.append_child(pugi::node_declaration);
    declaration.append_attribute("version") = "1.0";
    declaration.append_attribute("encoding") = "UTF-8";

    pugi::xml_parse_result result = doc.load_file("../../src/device-model-plugins/states_binary.xml");
    pugi::xml_node all_states = doc.first_child();

    if (all_states.empty())
    {
        pugi::xml_node all_states = doc.append_child("all_states");
        string tile_name = "tile_" + std::to_string(tile_id);
        pugi::xml_node tile = all_states.append_child(tile_name.c_str());

        for (size_t c = 0; c < this->num_col + this->num_imm + 2; c+=1)
        {
          string col_name = "col_" + std::to_string(c);
          pugi::xml_node col = tile.append_child(col_name.c_str());
          string col_state;
          if (c < this->num_col)
            col_state = this->columns[c].raw;
          else if ((c >= this->num_col) && (c < this->num_col + this->num_imm))
            col_state = this->immediates[c - this->num_col].raw;
          else if (c == this->num_col + this->num_imm) 
          {
            string all_zero = "";
            for (size_t i = 0; i < this->num_row; i++) {
              all_zero += "1";
            }
            col_state = all_zero;
          }
          else 
          {
            string all_one = "";
            for (size_t i = 0; i < this->num_row; i++) {
              all_one += "0";
            }
            col_state = all_one;
          }
          col.append_child(pugi::node_pcdata).set_value(col_state.c_str());
        }
    }
    else
    {
        pugi::xml_node all_states = doc.child("all_states");
        string tile_name = "tile_" + std::to_string(tile_id);
        pugi::xml_node tile = all_states.child(tile_name.c_str());

        if (tile.empty())
        {
            pugi::xml_node tile = all_states.append_child(tile_name.c_str());

            for (size_t c = 0; c < this->num_col + this->num_imm + 2; c+=1)
            {
              string col_name = "col_" + std::to_string(c);
              pugi::xml_node col = tile.append_child(col_name.c_str());
              string col_state;
              if (c < this->num_col)
                col_state = this->columns[c].raw;
              else if ((c >= this->num_col) && (c < this->num_col + this->num_imm))
                col_state = this->immediates[c - this->num_col].raw;
              else if (c == this->num_col + this->num_imm) 
              {
                string all_zero = "";
                for (size_t i = 0; i < this->num_row; i++) {
                  all_zero += "1";
                }
                col_state = all_zero;
              }
              else 
              {
                string all_one = "";
                for (size_t i = 0; i < this->num_row; i++) {
                  all_one += "0";
                }
                col_state = all_one;
              }
              col.append_child(pugi::node_pcdata).set_value(col_state.c_str());
            }
        }
    }

    doc.save_file("../../src/device-model-plugins/states_binary.xml");
}

void Tile::toggle_device_model_sim(bool device_model_sim_, bool memorisation_, string cycle_time, string volt_MAGIC, string volt_ISO_BL) 
{
	this->device_model_sim = device_model_sim_;
  this->memorisation = memorisation_;
	
	// initialization
	if (this->device_model_sim) 
  {
    if (this->memorisation)
    {
      this->cycle_time = cycle_time;
      this->volt_MAGIC = volt_MAGIC;
      this->volt_ISO_BL = volt_ISO_BL;
      Tile::update_states_binary(this->tile_id);
      Tile::update_states(this->tile_id);
    }
    else
    {
      for (size_t c = 0; c < this->num_col + this->num_imm; c+=1) 
      {
        wchar_t* stimuli[5];	
        stimuli[0] = L"../../src/device-model-plugins/Py_Tile.py";
        stimuli[1] = L"INIT";
        stimuli[2] = this->string2widechar(to_string(this->tile_id)); 
        stimuli[3] = this->string2widechar(to_string(c)); 
        if (c < this->num_col) {
          stimuli[4] = this->string2widechar(this->columns[c].raw);
        }
        
        else if ((c >= this->num_col) && (c < this->num_col + this->num_imm)) {
          stimuli[4] = this->string2widechar(this->immediates[c - this->num_col].raw);
        }
        
        else if (c == this->num_col + this->num_imm) {
          string all_zero = "";
          for (size_t i = 0; i < this->num_row; i++) {
            all_zero += "1";
          }
          stimuli[4] = string2widechar(all_zero);
        }
        
        else {
          string all_one = "";
          for (size_t i = 0; i < this->num_row; i++) {
            all_one += "0";
          }
          stimuli[4] = string2widechar(all_one);
        }
        
        this->simulate_verilogA(stimuli);
      }
    }
  }
}

/**
* @return true if this tile is MSB
* @return false if this tile is not MSB
*/
bool Tile::is_MSB() {
  return MSB_tile;
}

/**
* @brief Function called by Bit_Pipeline to set a tile to be MSB for using the torus network
*/
void Tile::set_MSB(bool MSBtile_) {
  this->MSB_tile = MSBtile_;
}

/**
* @brief Display the activity of the tile
*/
// void Tile::report() {
//   for (size_t i = 0; i < this->num_col; i+=1) {
//     cout << "Column " << this->columns[i].ID << endl;
//     this->columns[i].report();
//   }
// }

/**
* @brief Reset the tile to all 0, except for column c1, and deactive predication if it is on
*/
void Tile::reset() {
  for (size_t i = 0; i < this->num_col; i+=1) {
    this->columns[i].reset();
  }
  for (size_t i = 0; i < this->num_imm; i+=1) {
    this->immediates[i].reset();
  }
  for (size_t i = 0; i < this->num_mask; i+=1) {
    this->masks[i].reset();
  }
  deactivate_predication();
}

/**
* @brief Decode a column name
*
* All columns are prefixed by either "r", "i", "m", or "c", corresponding to
* regular, intermediate, mask, or constant columns respectively. This is followed
* by the index of the column (e.g., r32 refers to regular column number 33 ... we start from r0)
*
* The function can also return the L - left buffer or R - right buffer. The T bufer is the
* special buffer shared by LSB and MSB tile.
*
* @return the pointer to the correct column
*/
Column * Tile::select_column(string column_name) {
  char type = column_name[0];

  if (type == 'L') {
    if (MSB_tile == true) {
      return NULL;
    }
    return this->left_buffer;
  }
  else if (type == 'T') { // torus register
    return this->left_buffer;
  }
  else if (type == 'R') {
    return this->right_buffer;
  }

  size_t addr = stoi(column_name.substr(1, column_name.size() - 1));
  if (type == 'r') {
    return &(this->columns[addr]);
  }
  else if (type == 'c') {
    return &(this->constants[addr]);
  }
  else if (type == 'm') {
    return &(this->masks[addr]);
  }
  else if (type == 'i') {
    return &(this->immediates[addr]);
  }
  return NULL;
}

wchar_t* Tile::string2widechar(const string& narrowString) {
    size_t size = narrowString.length() + 1; // +1 to include null terminator

    // Allocate memory for the wide string
    wchar_t* wideString = new wchar_t[size];

    // Convert the narrow string to wide string
    mbstowcs(wideString, narrowString.c_str(), size);

    return wideString;
}

void Tile::generate_verilogA_LOGIC_memorisation(Racer_Uop uop, std::vector<std::string>& stimuli)
{
  stimuli[0] = uop.command;
  stimuli[1] = to_string(this->tile_id);

  string colA = uop.fields[1];
	string colB = uop.fields[2];
	string colC = uop.fields[3];
	
	size_t addrA = stoi(colA.substr(1, colA.size() - 1));
	size_t addrB = stoi(colB.substr(1, colB.size() - 1));
	size_t addrC = stoi(colC.substr(1, colC.size() - 1));
	
	if (colA[0] == 'i') {
		addrA += this->num_col;
	}
  else if (colA[0] == 'c') {
		addrA += this->num_col+this->num_imm;
	}
	if (colB[0] == 'i') {
		addrB += this->num_col;
	}
  else if (colB[0] == 'c') {
		addrB += this->num_col+this->num_imm;
	}
	if (colC[0] == 'i') {
		addrC += this->num_col;
	}
  else if (colC[0] == 'c') {
		addrC += this->num_col+this->num_imm;
	}
	
	stimuli[2] = to_string(addrA); 
	stimuli[3] = to_string(addrB); 
	stimuli[4] = to_string(addrC); 
  stimuli[5] = this->cycle_time;
  stimuli[6] = this->volt_MAGIC;
  stimuli[7] = this->volt_ISO_BL;

}

void Tile::generate_verilogA_LOGIC(Racer_Uop uop, wchar_t ** stimuli) {
	stimuli[0] = L"../../src/device-model-plugins/Py_Tile.py";	
	stimuli[1] = this->string2widechar(uop.command);		
	stimuli[2] = this->string2widechar(to_string(this->tile_id)); 
	 
	string colA = uop.fields[1];
	string colB = uop.fields[2];
	string colC = uop.fields[3];
	
	size_t addrA = stoi(colA.substr(1, colA.size() - 1));
	size_t addrB = stoi(colB.substr(1, colB.size() - 1));
	size_t addrC = stoi(colC.substr(1, colC.size() - 1));
	
	if (colA[0] == 'i') {
		addrA += this->num_col;
	}
  else if (colA[0] == 'c') {
		addrA += this->num_col+this->num_imm;
	}
	if (colB[0] == 'i') {
		addrB += this->num_col;
	}
  else if (colB[0] == 'c') {
		addrB += this->num_col+this->num_imm;
	}
	if (colC[0] == 'i') {
		addrC += this->num_col;
	}
  else if (colC[0] == 'c') {
		addrC += this->num_col+this->num_imm;
	}
	
	stimuli[3] = this->string2widechar(to_string(addrA)); 
	stimuli[4] = this->string2widechar(to_string(addrB)); 
	stimuli[5] = this->string2widechar(to_string(addrC)); 
}

void Tile::generate_verilogA_WRITE_memorisation(Racer_Uop uop, std::vector<std::string>& stimuli) 
{
	stimuli[0] = "WRITE";		
	stimuli[1] = std::to_string(this->tile_id); 
  
	size_t coladdr = stoi(uop.fields[2].substr(1, uop.fields[2].size() - 1));
	if (uop.fields[2][0] == 'i') {
		coladdr += this->num_col;
	}
  else if (uop.fields[2][0] == 'c') {
		coladdr += this->num_col+this->num_imm;
	}
	stimuli[2] = std::to_string(coladdr);
	
	Column * col = this->select_column(uop.fields[2]);
	stimuli[3] = col->raw; 
}

void Tile::generate_verilogA_WRITE(Racer_Uop uop, wchar_t ** stimuli) {
	stimuli[0] = L"../../src/device-model-plugins/Py_Tile.py";	
	stimuli[1] = L"WRITE";		
	stimuli[2] = this->string2widechar(to_string(this->tile_id)); 

	size_t coladdr = stoi(uop.fields[2].substr(1, uop.fields[2].size() - 1));
	if (uop.fields[2][0] == 'i') {
		coladdr += this->num_col;
	}
  else if (uop.fields[2][0] == 'c') {
		coladdr += this->num_col+this->num_imm;
	}
	stimuli[3] = this->string2widechar(to_string(coladdr));
	
	Column * col = this->select_column(uop.fields[2]);
	stimuli[4] = this->string2widechar(col->raw); 
}

void Tile::update_tile_state() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("../../src/device-model-plugins/states_binary.xml");

    if (result) {
        // Access root node
        pugi::xml_node root = doc.child("all_states");
        pugi::xml_node this_tile = root.child(("tile_" + to_string(this->tile_id)).c_str());
        
        for (size_t c = 0; c < this->num_col + this->num_imm; c+=1) {
        	string column_binarized_data = this_tile.child(("col_" + to_string(c)).c_str()).child_value();
        	
        	if (c < this->num_col) {
        		this->columns[c].write(column_binarized_data);
        	}
        	
        	else {
        		this->immediates[c - this->num_col].write(column_binarized_data);
        	}
        }

    } 
    else {
        std::cout << "XML file loading failed. Error description: " << result.description() << std::endl;
    }
}

/**
* @brief Execute a micro-op using the correct columns
*
* The predicated execution is internally realized as a MUX in the simulation.
* The tile uses different Boolean operators ~, |, &, ^, *, + to perform the in-memory primitives
*/
void Tile::execute(Racer_Uop uop) {
  if (uop.type == BOOLEAN) {

    Column * colA = select_column(uop.fields[1]);
    Column * colB = select_column(uop.fields[2]);
    Column * colC = select_column(uop.fields[3]);
    
    // For Full-adder primitive
    Column * colCARRY = select_column("R");

    if (colA == NULL || colB == NULL || colC == NULL) {
      return;
    }

    // Execute command
    if (this->predicated) {
      Column * M = this->active_mask;
      Column Mnot = ~(*(M));
      Column not_taken = Mnot * *(colC);
      if (uop.command == "NOR"){
        Column tmp0 = *(colA) | *(colB);
        Column taken = *(M) * tmp0;
        *(colC) = taken + not_taken;

        this->primitive_op_count["NOR"] += 1;
      }
      else if (uop.command == "NAND"){
        Column tmp0 = *(colA) & *(colB);
        Column taken = *(M) * tmp0;
        *(colC) = taken + not_taken;

        this->primitive_op_count["NAND"] += 1;
      }
      else if (uop.command == "OR"){
        Column tmp0 = *(colA) + *(colB);
        Column taken = *(M) * tmp0;
        *(colC) = taken + not_taken;

        this->primitive_op_count["OR"] += 1;
      }
      else if (uop.command == "AND"){
        Column tmp0 = *(colA) * *(colB);
        Column taken = *(M) * tmp0;
        *(colC) = taken + not_taken;

        this->primitive_op_count["AND"] += 1;
      }
      else if (uop.command == "XOR"){
        Column tmp0 = *(colA) ^ *(colB);
        Column taken = *(M) * tmp0;
        *(colC) = taken + not_taken;

        this->primitive_op_count["XOR"] += 1;
      }
      else if (uop.command == "FA") {
      	Column tmp0 = *(colA) ^ *(colB);
      	Column tmp1 = *(colA) * *(colB);
      	Column tmp2 = *(colCARRY) * tmp0;
      	Column tmp3 = tmp0 ^ *(colCARRY); // Sum
      	Column tmp4 = tmp1 + tmp2; // Carry
      	
      	Column taken_sum = *(M) * tmp3;
      	Column taken_carry = *(M) * tmp4;
      	Column not_taken_carry = Mnot * *(colCARRY);
      	
      	*(colC) = taken_sum + not_taken;
      	*(colCARRY) = taken_carry + not_taken_carry;
      	
      	// colA->read_op_count["XOR"] -= 1;
      	// colB->read_op_count["XOR"] -= 1;
      	// colA->read_op_count["AND"] -= 1;
      	// colB->read_op_count["AND"] -= 1;
      	
      	// colA->read_op_count["FA"] += 1;
      	// colB->read_op_count["FA"] += 1;
      	// colC->write_op_count["FA"] += 1;

        this->primitive_op_count["FA"] += 1;
      }
      // Need to readjust the log because some of the MUX Boolean ops are not real
      // colC->read_op_count["AND"] -= 1;
      // colC->write_op_count["OR"] -= 1;
      // M->read_op_count["NOT"] -= 1;
      // M->read_op_count["AND"] -= 1;
    }
    else 
    { // regular execution
      if (uop.command == "NOR"){
        *(colC) = *(colA) | *(colB);
        this->primitive_op_count["NOR"] += 1;
      }
      else if (uop.command == "NAND"){
        *(colC) = *(colA) & *(colB);
        this->primitive_op_count["NAND"] += 1;
      }
      else if (uop.command == "OR"){
        *(colC) = *(colA) + *(colB);
        this->primitive_op_count["OR"] += 1;
      }
      else if (uop.command == "AND"){
        *(colC) = *(colA) * *(colB);
        this->primitive_op_count["AND"] += 1;
      }
      else if (uop.command == "XOR"){
        *(colC) = *(colA) ^ *(colB);
        this->primitive_op_count["XOR"] += 1;
      }
      else if (uop.command == "FA") {
      	Column tmp0 = *(colA) ^ *(colB);
      	Column tmp1 = *(colA) * *(colB);
      	Column tmp2 = *(colCARRY) * tmp0;
      	*(colC) = tmp0 ^ *(colCARRY);
      	*(colCARRY) = tmp1 + tmp2;
        this->primitive_op_count["FA"] += 1;
      }

      // If simulate using VerilogA model
    	if (this->device_model_sim) 
      {
        if (this->memorisation)
        {
          string ideal_value = colC->raw;

          std::vector<std::string> stimuli_memorisation(8);
          this->generate_verilogA_LOGIC_memorisation(uop, stimuli_memorisation);
          this->simulate_verilogA_memorisation(stimuli_memorisation);
          this->update_tile_state();

          string analog_value = select_column(uop.fields[3])->raw;
		
		      if (analog_value != ideal_value) {
			      this->analog_error_counter ++;
		      }
        }
        else
        {
          string ideal_value = colC->raw;
    		
		      wchar_t* stimuli[6];
		      this->generate_verilogA_LOGIC(uop, stimuli);
		      this->simulate_verilogA(stimuli);
		      this->update_tile_state();
		
		      string analog_value = select_column(uop.fields[3])->raw;
		
		      if (analog_value != ideal_value) {
			      this->analog_error_counter ++;
		      }
        }
    	}

    }
  }

  else if (uop.type == TRANSFER) {
    Column * src = select_column(uop.fields[1]);
    Column * des = select_column(uop.fields[2]);

    if (src == NULL || des == NULL) {
      return;
    }

    // Transfer
    if (this->predicated) {
      Column * M = this->active_mask;
      Column Mnot = ~(*(M));
      Column not_taken = Mnot * *(des);
      Column tmp0 = *(src);
      Column taken = *(M) * tmp0;
      *(des) = taken + not_taken;
      this->primitive_op_count["COPY"] += 1;
      // des->read_op_count["AND"] -= 1;
      // des->write_op_count["OR"] -= 1;
      // M->read_op_count["NOT"] -= 1;
      // M->read_op_count["AND"] -= 1;
    }
    else 
    {
      des->copy(src);
      if (this->device_model_sim && (uop.fields[1][0] == 'R')) 
      { // only simulate case where reading from right buffer
        if (this->memorisation)
        {
          // cout << "Transfering binarized buffer to verilogA flow " << uop.raw << endl;
          std::vector<std::string> stimuli_memorisation(4);
          this->generate_verilogA_WRITE_memorisation(uop, stimuli_memorisation);
	        this->simulate_verilogA_memorisation(stimuli_memorisation);
          this->update_tile_state();
        }
        else
        {
          wchar_t* stimuli[5];	
          this->generate_verilogA_WRITE(uop, stimuli);
	        this->simulate_verilogA(stimuli);
          this->update_tile_state();
        }
      }
    }
  }

  else if (uop.type == PREDICATE) {
    if (uop.command == "UNMASK") {
      this->deactivate_predication();
    }
    else {
      this->activate_predication(uop.fields[1]);
    }
  }
}

/**
* @brief write data to a tile
* @param data is a 2D object, each column in the object is one column in the tile
*/
void Tile::write_data(vector <string> data) {
  if (data.size() != this->num_col) {
    printf("TILE: ERROR: loading data size different from no. column\n");
    exit(-1);
  }

  for (size_t i = 0; i < this->num_col; i+=1) {
    this->columns[i].write(data[i]);
  }
}

/**
* @brief activate predication in the tile
* @param mask_id the address of the column where the bit mask is located
*/
void Tile::activate_predication(string mask_id) {
  this->active_mask = select_column(mask_id);
  this->predicated = true;
}

/**
* @brief deactivate predication in the tile
*/
void Tile::deactivate_predication() {
  this->active_mask = &(this->constants[1]); // just in case
  this->predicated = false;
}

/**
* @brief Display the tile data for debugging
*
* From left to right, it will display regular columns, intermediate columns, mask columns, and the right buffer
*/
void Tile::display_data() {
  for (size_t r = 0; r < this->num_row; r+=1) {
    for (size_t c = 0; c < this->num_col; c+=1) {
      cout << this->columns[c].field[r] << " ";
    }

    cout << " | ";

    for (size_t i = 0; i < this->num_imm; i+=1) {
      cout << this->immediates[i].field[r] << " ";
    }

    cout << " | ";

    for (size_t i = 0; i < this->num_mask; i+=1) {
      cout << this->masks[i].field[r] << " ";
    }

    cout << " | ";
    cout << this->right_buffer->field[r] << " ";
    cout << endl;
  }
}

/**
* @brief Get one single bit from the tile
*
* @param row row coordinate of the bit
* @param col col coordinate of the bit
*/
uint8_t Tile::get_bit(size_t row, size_t col){
  return this->columns[col].field[row];
};

