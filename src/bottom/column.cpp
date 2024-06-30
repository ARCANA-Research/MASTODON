#include "column.h"
/**
* @brief Constructor of Column that cast a raw binary string to a column
*/
Column::Column(string raw_, int col_size_) {
  this->field.clear();
  // this->write_op_count.insert({"AND", 0});
  // this->write_op_count.insert({"NAND", 0});
  // this->write_op_count.insert({"OR", 0});
  // this->write_op_count.insert({"NOR", 0});
  // this->write_op_count.insert({"XOR", 0});
  // this->write_op_count.insert({"NOT", 0});
  // this->write_op_count.insert({"COPY", 0});
  // this->write_op_count.insert({"FA", 0});
  // //this->write_op_count.insert({"MAJ3", 0});
  
  // this->read_op_count.insert({"FA", 0});
  // //this->read_op_count.insert({"MAJ3", 0});
  // this->read_op_count.insert({"AND", 0});
  // this->read_op_count.insert({"NAND", 0});
  // this->read_op_count.insert({"OR", 0});
  // this->read_op_count.insert({"NOR", 0});
  // this->read_op_count.insert({"XOR", 0});
  // this->read_op_count.insert({"NOT", 0});
  // this->read_op_count.insert({"COPY", 0});
  if (col_size_ > MAX_COL_SIZE) {
    printf("Column: ERROR: initiating column larger than MAX_COL_SIZE\n");
  }
  this->col_size = col_size_;
  this->raw = raw_;
  this->binary = stoul(raw_, 0, 2);
  for (size_t i = 0; i < raw_.size(); i+=1) {
    if (raw_[i] == '0'){
      this->field.push_back(0);
    }
    else if (raw_[i] == '1') {
      this->field.push_back(1);
    }
    else {
      printf("Column: ERROR: Attempting to store non-binary \n");
      exit(-1);
    }
  }
}

/**
* @brief Constructor of Column that cast an unsigned char word into a binary column
*/
Column::Column(size_t binary_, int col_size_) {
  this->field.clear();
  // this->write_op_count.insert({"AND", 0});
  // this->write_op_count.insert({"NAND", 0});
  // this->write_op_count.insert({"OR", 0});
  // this->write_op_count.insert({"NOR", 0});
  // this->write_op_count.insert({"XOR", 0});
  // this->write_op_count.insert({"NOT", 0});
  // this->write_op_count.insert({"FA", 0});
  // //this->write_op_count.insert({"MAJ3", 0});
  
  // this->read_op_count.insert({"FA", 0});
  // //this->read_op_count.insert({"MAJ3", 0});
  // this->read_op_count.insert({"AND", 0});
  // this->read_op_count.insert({"NAND", 0});
  // this->read_op_count.insert({"OR", 0});
  // this->read_op_count.insert({"NOR", 0});
  // this->read_op_count.insert({"XOR", 0});
  // this->read_op_count.insert({"NOT", 0});
  if (col_size_ > MAX_COL_SIZE) {
    printf("Column: ERROR: initiating column larger than MAX_COL_SIZE\n");
  }
  this->col_size = col_size_;
  this->binary = binary_;
  this->raw = bitset<MAX_COL_SIZE>(binary_).to_string();
  this->raw = this->raw.substr(this->raw.size() - this->col_size, this->col_size); // string conversion
  for (size_t i = 0; i < this->raw.size(); i+=1) {
    // this->field.push_back(atoi(&(this->raw[i])));
    if (this->raw[i] == '0'){
      this->field.push_back(0);
    }
    else if (this->raw[i] == '1') {
      this->field.push_back(1);
    }
    else {
      printf("Column: ERROR: Attempting to store non-binary \n");
      exit(-1);
    }
  }
}

/**
* @brief Constructor of Column that cast a vector of binary 0 or 1 integer into a column
*/
Column::Column(vector<int> field_, int col_size_) {
  this->field.clear();
  // this->write_op_count.insert({"AND", 0});
  // this->write_op_count.insert({"NAND", 0});
  // this->write_op_count.insert({"OR", 0});
  // this->write_op_count.insert({"NOR", 0});
  // this->write_op_count.insert({"XOR", 0});
  // this->write_op_count.insert({"NOT", 0});
  // this->write_op_count.insert({"FA", 0});
  // //this->write_op_count.insert({"MAJ3", 0});
  
  // this->read_op_count.insert({"FA", 0});
  // //this->read_op_count.insert({"MAJ3", 0});
  // this->read_op_count.insert({"AND", 0});
  // this->read_op_count.insert({"NAND", 0});
  // this->read_op_count.insert({"OR", 0});
  // this->read_op_count.insert({"NOR", 0});
  // this->read_op_count.insert({"XOR", 0});
  // this->read_op_count.insert({"NOT", 0});
  if (col_size_ > MAX_COL_SIZE) {
    printf("Column: ERROR: initiating column larger than MAX_COL_SIZE\n");
  }
  this->col_size = col_size_;
  this->raw = "";
  for (size_t i = 0; i < field_.size(); i+=1) {
    this->raw.append(to_string(field_[i]));
  }
  this->binary = stoul(this->raw, 0, 2);
  this->field = field_;
}

/**
* @brief write a vector of 0 or 1 integer into the column
*/
void Column::write(vector<int> field_) {
  this->field.clear();
  this->raw = "";
  for (size_t i = 0; i < field_.size(); i+=1) {
    this->raw.append(to_string(field_[i]));
  }
  this->binary = stoul(this->raw, 0, 2);
  this->field = field_;
}

/**
* @brief write a binary string into the column
*/
void Column::write(string raw_) {
  this->field.clear();
  this->raw = raw_;
  this->binary = stoul(raw_, 0, 2);
  for (size_t i = 0; i < raw_.size(); i+=1) {
    if (raw_[i] == '0'){
      this->field.push_back(0);
    }
    else if (raw_[i] == '1') {
      this->field.push_back(1);
    }
    else {
      printf("Column: ERROR: Attempting to store non-binary \n");
      exit(-1);
    }
  }
}

/**
* @brief write an unsigned char word into the column
*/
void Column::write(size_t binary_) {
  this->field.clear();
  this->binary = binary_;
  this->raw = bitset<MAX_COL_SIZE>(binary_).to_string();
  this->raw = this->raw.substr(this->raw.size() - this->col_size, this->col_size); // string conversion
  for (size_t i = 0; i < this->raw.size(); i+=1) {
    // this->field.push_back(atoi(&(this->raw[i])));
    if (this->raw[i] == '0'){
      this->field.push_back(0);
    }
    else if (this->raw[i] == '1') {
      this->field.push_back(1);
    }
    else {
      printf("Column: ERROR: Attempting to store non-binary \n");
      exit(-1);
    }
  }
}

/**
* @brief write 0s column
*/
void Column::reset() {
  this->write(0);
}

/**
* @brief Binary operator overloading: Boolean OR
*/
Column Column::operator + (Column & obj) {
  Column ret (this->binary | obj.binary, this->col_size);
  // this->read_op_count["OR"] +=1;
  // obj.read_op_count["OR"] +=1;
  // ret.write_op_count["OR"] +=1;
  return ret;
}

/**
* @brief Binary operator overloading: Boolean AND
*/
Column Column::operator * (Column & obj) {
  Column ret (this->binary & obj.binary, this->col_size);
  // this->read_op_count["AND"] +=1;
  // obj.read_op_count["AND"] +=1;
  // ret.write_op_count["AND"] +=1;
  return ret;
}

/**
* @brief Binary operator overloading: Boolean NOR
*/
Column Column::operator | (Column & obj) {
  Column ret (~(this->binary | obj.binary), this->col_size);
  // this->read_op_count["NOR"] +=1;
  // obj.read_op_count["NOR"] +=1;
  // ret.write_op_count["NOR"] +=1;
  return ret;
}

/**
* @brief Binary operator overloading: Boolean NAND
*/
Column Column::operator & (Column & obj) {
  Column ret (~(this->binary & obj.binary), this->col_size);
  // this->read_op_count["NAND"] +=1;
  // obj.read_op_count["NAND"] +=1;
  // ret.write_op_count["NAND"] +=1;
  return ret;
}

/**
* @brief Uniary operator overloading: Boolean NOT
*/
Column Column::operator ~ () {
  Column ret (~(this->binary), this->col_size);
  // ret.write_op_count["NOT"] +=1;
  return ret;
}

/**
* @brief Binary operator overloading: Boolean XOR
*/
Column Column::operator ^ (Column & obj) {
  Column ret (this->binary ^ obj.binary, this->col_size);
  // this->read_op_count["XOR"] +=1;
  // obj.read_op_count["XOR"] +=1;
  // ret.write_op_count["XOR"] +=1;
  return ret;
}

/**
* @brief Uniary operator overloading: copy
*/
Column Column::operator = (Column result) {
  // this->write_op_count["AND"] += result.write_op_count["AND"];
  // this->write_op_count["OR"] += result.write_op_count["OR"];
  // this->write_op_count["NAND"] += result.write_op_count["NAND"];
  // this->write_op_count["NOR"] += result.write_op_count["NOR"];
  // this->write_op_count["NOT"] += result.write_op_count["NOT"];
  // this->write_op_count["XOR"] += result.write_op_count["XOR"];
  // this->raw = result.raw;
  this->write(result.binary);
  return *this;
}

/**
* @brief Index operator overloading: return a single bit in a column
*/
int Column::operator [] (size_t idx) {
  return this->field[idx];
}

/**
* @brief Stream operator overloading: print out the column
*/
ostream & operator << (ostream & os, Column const & col) {
  os << col.raw.substr(MAX_COL_SIZE - col.col_size, col.col_size);
  return os;
}

void Column::copy(Column * src){
  // this->write_op_count["COPY"] += 1;
  // src->read_op_count["COPY"] += 1;
  this->write(src->binary);
};

void Column::maj3(Column * src1, Column * src2) {
  size_t tmp0 = this->binary & src1->binary;
  size_t tmp1 = src1->binary & src2->binary;
  size_t tmp2 = this->binary & src2->binary;

  size_t maj3_res = tmp0 | tmp1 | tmp2;
  this->write(maj3_res);
  src1->write(maj3_res);
  src2->write(maj3_res);

  // this->write_op_count["MAJ3"] += 1;
  // this->read_op_count["MAJ3"] += 1;

  // src1->write_op_count["MAJ3"] += 1;
  // src1->read_op_count["MAJ3"] += 1;

  // src2->write_op_count["MAJ3"] += 1;
  // src2->read_op_count["MAJ3"] += 1;
};

/**
* @brief Display the WRITE/READ report of the columns
*
* The WRITE report contains the count for each Boolean operations that the column is the
* output. Similarly, READ report contains the count of each Boolean operations that the column is an input
*/
// void Column::report() {
//   cout << "WRITE" << endl;
//   for (auto const &pair: this->write_op_count) {
//     cout << "{" << pair.first << ": " << pair.second << "}" <<endl;
//   }
//   cout << "READ" << endl;
//   for (auto const &pair: this->read_op_count) {
//     cout << "{" << pair.first << ": " << pair.second << "}" <<endl;
//   }
// };
