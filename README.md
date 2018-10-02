# Benchmarking Caches

## How it works 
This was done by just me. 
My program is split into a few parts, each trying a different strategy to trick the prefetcher. The goal of this project is to note the differences in time to copy a piece of data from a buffer, given it is in a different cache layer. The program basically consists of a function that generates a buffer with random data, then it creates a vector of pointers randomly pointing to different elements of the buffer. Further it then creates a list of random numbers and tries to perform an algorithimic way of getting 100 elements from the buffer in such a random way that even the prefetcher can't figure it out.  


## Running the tests

I ran all tests using g++ -O3 -Wall -g buff.cc

### 1  
This graph is parabolic, however it definitely has steps at the cpu caches. Given the data, it seems the L1 + L2 cache is probably smaller then 2097k, and from there it starts preloading into RAM, my computer has no L3 cache. It probably has an L2 cache of size around 1024k and an L1 of somewhere between 50k and 500k, given that the jumps start at 2097k.

### 2
The performance was not the same as these numbers, likely because the calculation include a small overhead, however if you divide out the number of copies I am doing (around 10) and a small overhead, you get around this measurements.


### 3
The L1d cache is 24k, L1i - 32k and L2 - 1024k. Those numbers compare to my measurements 

