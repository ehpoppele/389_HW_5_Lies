#include "driver.hh"

//The constructor doesn't do much, since we leave most of the heavy lifting on distribution generation etc to the warm method
Driver::Driver(Cache cache)
{
    cache_ = cache;
    total_prob_ = 0;
    hits_ = 0;
    misses_ = 0;
    //set vecs to empty?
}

Driver::~Driver()
{
    //reset the cache? I forgot what this does
}

//warm generates new data and adds to vector until cache has gotten at least size total data
Driver::warm(int size)
{
    total_prob_ = 0;
    //empty the data vec
    int size_used = 0;
    int i = 0;
    while(size_used < 2*size){//we want the cache full and still plenty of pairs left; pointless to test if cache can hold 100% of data
        //First generate a new data tuple based on our distributions
        int key_size = 1; //should be normal dist between 20 and 70
        int padding = log_10(1000000 - i)//since cache has max memory of ~1mil bytes, should never need more than that many keys
        int filler = key_size - 10; //10 chars needed for "key_000000"; then the rest is filler "f"
        key_type key = "key_" + std::string(padding, '0') + std::to_string(i) + std::string(filler, 'f');//f for filler and f to pay respects to all those wasted bytes

        int val_size = 1;//should be
        std::string val_str = std::string(val_size, 'B');//the values for entries are just the character "B" repeated up to 5000 times
        val_type val = val_str.c_str();
        size_type size = val_size
        size_used += val_size;
        //generate a new data tuple
        //append new data tuple
        //update size and prob
        //call set with new kv pair
        i++;
    }
    //shuffle data vector
}

//genreq draws from vector based on distribution
