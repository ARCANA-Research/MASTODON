#include "mat.h"

  Mat::Mat(string config_file_addr_, size_t ID_) {
  	this->ID = ID_;
  	this->num_col =  stoull(util::general_config_parser("num_col", config_file_addr_));
  	this->num_row =  stoull(util::general_config_parser("num_row", config_file_addr_));
  	this->granularity =  stoull(util::general_config_parser("granularity", config_file_addr_));

  	this->active = false;

  	string all_zeros(this->num_row, '0');
  	string all_ones(this->num_row, '1');
  	this->c_group.push_back(new Column(all_zeros, this->num_row));
  	this->c_group.push_back(new Column(all_ones, this->num_row));

  	for (size_t i = 0; i < this->num_col; i+=1) {
  		this->d_group.push_back(new Column(all_zeros, this->num_row));
  	}

  	this->T0 = new Column(all_zeros, this->num_row);
  	this->T1 = new Column(all_zeros, this->num_row);
  	this->T2 = new Column(all_zeros, this->num_row);
  	this->T3 = new Column(all_zeros, this->num_row);

  	this->DCC0 = new Column(all_zeros, this->num_row);
  	this->DCC1 = new Column(all_zeros, this->num_row);

  	this->DCC0_bar = new Column(all_ones, this->num_row);
  	this->DCC1_bar = new Column(all_ones, this->num_row);

  	this->primitive_op_count.insert({"RowClone", 0});
  	this->primitive_op_count.insert({"AmbitMAJ", 0});
  };

void Mat::toggle_device_model_sim(bool device_model_sim, bool memorisation, string cycle_time, string volt_MAGIC, string volt_ISO_BL)
{
	
}

void Mat::write_data(vector <string> data){

	// check that the input data has the same dimension as the mat
	if (this->num_col != data.size() || this->num_row != data[0].size()) {
		printf("ERROR: Mat: writing data dimension is not the same as Mat\n");
		exit(-1);
	}

	for (size_t d = 0; d < this->num_col; d += 1) {
		this->d_group[d]->write(data[d]);
	}
};

void Mat::display_data(){
	int size;
	if (this->num_col > this->num_row) {
		size = this->num_col;
	}
	else {
		size = this->num_row;
	}
	char transpose_data[size][size];
	for (size_t c = 0; c < this->num_col; c+= 1) {
		string col_data = this->d_group[c]->raw;
		for (size_t r = 0; r < this->num_row; r += 1) {
			transpose_data[c][r] = col_data[r];
		}
	}

	cout << "Dual Contact | B Group | D Group" << endl;
	for (size_t r = 0; r < this->num_row; r+= 1) {
		cout << this->DCC0->raw[r] << " " << this->DCC1->raw[r]  << " | " << this->T0->raw[r] << " " << this->T1->raw[r] << 
		" " << this->T2->raw[r] << " " << this->T3->raw[r] << " | ";
		for (size_t c = 0; c < this->num_col; c += 1) {
			if (c > 64) {
				cout << "...";
				break;
			}
			cout << transpose_data[c][r] << " ";
		}
		cout << endl;
	}
};

Column * Mat::fetch_torus_register(int __dummy_do_not_use__) {
	return this->DCC0;
}

Column * Mat::fetch_regular_register(int __dummy_do_not_use__, int reg_idx, int bit_pos) {
	return this->d_group[reg_idx * this->granularity + bit_pos];
}

void Mat::show_log(bool show_){
	this->show = show_;
};

void Mat::toggle_backend(bool disable_signal_){
	this-> disable_signal = disable_signal_;
};

void Mat::record_log(bool record_){
	this->record = record_;
};

void Mat::set_log_node(pugi::xml_node parent_node_){
	this->parent_node = parent_node_;
};

bool Mat::is_active(){
	return this->active;
};

float Mat::report_activity_factor(){
	return 0.0;
};

// // mainly used for testing, probably should be modified further
// uint8_t Mat::get_byte(size_t bg, size_t row, size_t col) {
// 	return '\0';
// };

map <string, size_t> Mat::report_primitive_op_count(){
	return this->primitive_op_count;
};

vector<Column *> Mat::select_columns(string ureg) {
	vector <Column *> ret;

if (ureg == "B_T0") {
	ret.push_back(this->T0);
}
else if (ureg == "B_T1") {
	ret.push_back(this->T1);
}
else if (ureg == "B_T2") {
	ret.push_back(this->T2);
}
else if (ureg == "B_T3") {
	ret.push_back(this->T3);
}
else if (ureg == "B_DCC0") {
	ret.push_back(this->DCC0);
}
else if (ureg == "B_DCC0b") {
	ret.push_back(this->DCC0_bar);
}
else if (ureg == "B_DCC1") {
	ret.push_back(this->DCC1);
}
else if (ureg == "B_DCC1b") {
	ret.push_back(this->DCC1_bar);
}
else if (ureg == "B_DCC0b_T0") {
	ret.push_back(this->DCC0_bar);
	ret.push_back(this->T0);
}
else if (ureg == "B_DCC1b_T1") {
	ret.push_back(this->DCC1_bar);
	ret.push_back(this->T1);
}
else if (ureg == "B_T2_T3") {
	ret.push_back(this->T2);
	ret.push_back(this->T3);
}
else if (ureg == "B_T0_T3") {
	ret.push_back(this->T0);
	ret.push_back(this->T3);
}
else if (ureg == "B_T0_T1_T2") {
	ret.push_back(this->T0);
	ret.push_back(this->T1);
	ret.push_back(this->T2);
}
else if (ureg == "B_T0_T1_T3") {
	ret.push_back(this->T0);
	ret.push_back(this->T1);
	ret.push_back(this->T3);

}
else if (ureg == "B_T1_T2_T3") {
	ret.push_back(this->T1);
	ret.push_back(this->T2);
	ret.push_back(this->T3);

}
else if (ureg == "B_DCC0_T1_T3") {
	ret.push_back(this->DCC0);
	ret.push_back(this->T1);
	ret.push_back(this->T3);
}
else if (ureg == "B_DCC1_T0_T2") {
	ret.push_back(this->DCC1);
	ret.push_back(this->T0);
	ret.push_back(this->T2);
}
else if (ureg == "B_DCC1_T0_T3") {
	ret.push_back(this->DCC1);
	ret.push_back(this->T0);
	ret.push_back(this->T3);
}
else if (ureg == "C0") {
	ret.push_back(this->c_group[0]);
}
else if (ureg == "C1") {
	ret.push_back(this->c_group[1]);
}
// if it is D_group
else if (ureg[0] == 'D') {
	int addr = stoi(ureg.substr(1));
	ret.push_back(this->d_group[addr]);
}
else {
	printf("ERROR: Mat: ureg name invalid\n");
	exit(-1);
}

return ret;
};

void Mat::execute(Mimdram_Uop uop){
	if (!this->disable_signal) {
		this->active = true;
		if (uop.type == MIMDRAM_UOP_RCLONE) {
			this->primitive_op_count["RowClone"] += 1;
			Column * src = this->select_columns(uop.fields[1])[0];
			vector <Column *> des = this->select_columns(uop.fields[2]);
			
			for (unsigned int i = 0; i < des.size(); i+=1) {
				des[i]->copy(src);
			}
		}

		else if (uop.type == MIMDRAM_UOP_MAJOR) {
			this->primitive_op_count["AmbitMAJ"] += 1;
			vector <Column *> operands = select_columns(uop.fields[1]);

			if (operands.size() != 3) {
				cout << "ERROR: Mat: Majority function must be applied to 3 inputs" << endl;
				exit(-1);
			}

			// take majoriy of three operands, overwrite all three with the majority vote result
			operands[0]->maj3(operands[1], operands[2]);
			
			// mimdram optimization of MAJ + Rowclone fusing
			if (uop.field_length == 3) {
				Column * d_column = select_columns(uop.fields[2])[0];
				d_column->copy(operands[0]);
			}
		}

		else if (uop.command == "DONE") {
			this->active = false;
		}

		// otherwise, micro-op is control arithmetic, so do nothing
		else {
			;
		}
	}
};

void Mat::update_dual_contact() {
	Column tmp = ~(*(this->DCC0));
	this->DCC0_bar->copy(&tmp);
	tmp = ~(*(this->DCC1));
	this->DCC1_bar->copy(&tmp);
};

void Mat::write_to_node() {
if (this->record == true) {
    this->this_node = this->parent_node.append_child(("Mat_" + to_string(this->ID)).c_str());
}
};

void Mat::tick() {
	this->write_to_node();
	this->update_dual_contact();
};
