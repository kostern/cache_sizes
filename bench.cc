#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;
using namespace std::chrono;

const int REPS = 10;
random_device rd;
mt19937 eng(rd());
uniform_int_distribution<> distr(0, INT_MAX);

//for consistency, we clear the entire L1, L2 and L3 cache using random numbers
void cache_clear(){
  const size_t bigger_than_cache = 1024000; //this corresponds to L1d of 24k + L2 of 1024k, all times two
  long *bank = new long[bigger_than_cache];
  memset(bank, 1, bigger_than_cache);
  for(int i = 0; i < bigger_than_cache; i++)
  {
    bank[i] = rand(); //fill it up with random numbers to try to hit each cache line
  }
  delete[] bank;
}

uint8_t* gen_buffer(size_t buff_size){
  uint8_t* data = new uint8_t[buff_size];
  memset(data, 1, buff_size); //prefill memory with 1, instead of 0 filled
  for(int i = 0; i < buff_size; i++) {
    data[i] = distr(eng); //fill up with random numbers, to ensure the memory is all filled up
  }
  return data;
}

std::vector<uint8_t*> shuffle_pointers(uint8_t* data, size_t size) {
  auto points = vector<uint8_t*>(size);
  for (size_t i = 0; i < size; ++i) {
    points[i] = &data[i];
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  shuffle(points.begin(), points.end(), std::default_random_engine(seed));
  return points;
}

std::vector<uint32_t> gen_rands(size_t size) {
  vector<uint32_t> rands; 
  for(size_t i = 0; i < size; i++) {
    uint32_t r = distr(eng) % (size/10);
    rands.push_back(r);
  }
  return rands;
}

void do_work(vector<uint8_t*> pointers, vector<uint32_t> rands, int length) {
  int count = 0;
  int index = 0;
  while(count < 100) {
    if(index >= (length - (length/10)) ) { //increment using index as source
      index = rands[index]; //reset index
    } else{ 
      index += rands[index]; //increment by random number
    }

    //if even get from front, if odd get from back
    if(index % 2 == 0) {
      uint8_t x = *pointers[index];
    } else {
      uint8_t x = *pointers[length - index];
    }
    count++;
  }	  
}

int main() {
  for(int size = 10; size <= 26; size++){
    cache_clear(); 
    //initialize buffer of 2 to the power of size
    size_t buff_size = pow(2.0, size); 
    uint8_t* data = gen_buffer(buff_size);

    //get shuffled array of pointers to locations of data
    auto pointers = shuffle_pointers(data, buff_size);
    
    double total_ns_time = 0;
    for(int i = 0; i < REPS; i++){ //start repeats 
      vector<uint32_t> rands = gen_rands(buff_size);
      const auto start = steady_clock::now();
      do_work(pointers, rands, buff_size);
      const auto end = steady_clock::now();
      double ns_time = 1000000000*((duration_cast<duration<double>>(end - start).count())/(100));
      total_ns_time += ns_time;
    }
    
    cout << "Time to run with buffer of ";
    std::string str_size = std::to_string(buff_size);
    cout << str_size;
    cout << " bytes: " << total_ns_time/REPS << "\n"; 

    delete[] data;
  }

  return 0;
}
