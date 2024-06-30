#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
/*
Generic data class that can be used to pass data around different source and
desitnation. For example, RACER binary cluster needs to send ISA instruction
to controller, and it will take a certain amount of time for some amount of data

typename D is usually byte (char), but this can change
*/

template <typename D> // data unit
class Data
{
public:
  int size; // data size
  int latency; // latency before data is available for read

  // Member functions
  Data();
  Data(int latency_, int size_, D* data_);
  void tick();
  int begin_countdown();
  int read_data(D* data);

//private:
  int timer; // countdown timer, timer == 0 means data can be accessed
  D* data;
};

template <typename D>
Data<D>::Data()
{
  this->size = -1;
  this->latency = -1;
  this->timer = -1; // -1 mean not available, 0 mean available, >0 mean moving
}

/**
* @brief Constructor for the Data<T> class
*
* @param latency_ the number of cycles needed for data to be ready
* @param size_ the number of bytes in the data
* @param data_ the pointer to the data itself
*
* Create a Data<T> object. This object is used to transfer data between two entities.
* It has a latency timer that will countdown until the data can be safely be read.
* It simulates the data transfering between two entities. It is mainly used to transfer
* data between external source vs. binary storage, and between binary storage vs. controller
*/
template <typename D>
Data<D>::Data(int latency_, int size_, D* data_)
{
  this->size = size_;
  this->latency = latency_;
  this->timer = -1; // -1 mean not available, 0 mean available, >0 mean moving
  this->data = (D*) malloc(sizeof(D) * size_);
  memcpy(this->data, data_, size_ * sizeof(D));
};

/**
* @brief Begin the timer countdown. When timer hits 0, then data can be safely read
*
* @return -1 means the timer has not expired
* @return 0 means the data can be read
*/
template <typename D>
int Data<D>::begin_countdown()
{
  if (this->timer > 0)
  {
    // printf("WARNING: Data object countdown in progress, create a new one \n");
    return -1;
  }

  this->timer = this->latency;
  return 0;
};

/**
* @brief Decrement the timer
*/
template <typename D>
void Data<D>::tick()
{
  if (this-> timer > 0)
  {
    this->timer -= 1;
  }
};

/**
* @brief Attempt to read the data object
*
* @param data the data pointer that the content of the object will be copied to
*
* @return -1 if the data is not ready to be read
* @return 0 means the data can be read, and the content is available in the data pointer
*/
template <typename D>
int Data<D>::read_data(D* data)
{
  if (this->timer == 0) {
    memcpy(data, this->data, this->size * sizeof(D));
    return 0;
  }
  // printf("timer is: %d\n", this->timer);
  // std::cout << "Data Bus: WARNING: cannot read data, wait for " << this->timer << " cycle(s)" << std::endl;;
  return -1;
}

#endif
