#include <algorithm>
#include <random>
#include "driver.hh"
#include <boost/math/distributions/pareto.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <cstring>

const int PERCENTILE_METRIC = 95;
//The constructor doesn't do much, since we leave most of the heavy lifting on distribution generation etc to the warm method
Driver::Driver(Cache* cache, Generator gen):gen_(gen)
{
    cache_ = cache;
}

Driver::~Driver()
{
    //reset the cache? I forgot what this does
}

//warm generates new data and adds to vector until cache has gotten at least size total data
//It creates tuples of key, val, size, and probability that are drawn from distributions designed to mimic the ETC workload
void Driver::warm(int size)
{
    int sets = 0;
    while(sets < size) {
        Request req = gen_.gen_req(false);
        if(req.method_ == "set") {
            cache_->set(req.key_, req.val_, strlen(req.val_));
            sets += 1;
        }
    }
}

void Driver::set_request(key_type key, Cache::val_type val, Cache::size_type size)
{
    cache_->set(key, val, size);
}

Cache::val_type Driver::get_request(key_type key)
{
    Cache::size_type size;
    return cache_->get(key, size);
}

bool Driver::del_request(key_type key)
{
    return cache_->del(key);
}

int Driver::head_request()
{
    return cache_->space_used();
}
//Delete driver data and reset cache as well
void Driver::reset()
{
    cache_->reset();
// warm generates new data and adds to vector until cache has gotten at least size total data
}

// param: number of requests to make
// return: vector containing the time for each measurement
std::vector<std::chrono::milliseconds> Driver::baseline_latencies(int nreq) {
    std::vector<std::chrono::milliseconds> results(nreq);
    std::chrono::time_point<std::chrono::high_resolution_clock> t1;
    std::chrono::time_point<std::chrono::high_resolution_clock> t2;
    int hits = 0;
    for(int i = 0; i < nreq; i++) {
        Request req = gen_.gen_req(false);
        Cache::size_type size = 0;
        if(req.method_ == "get") {
            Cache::val_type response;
            t1 = std::chrono::high_resolution_clock::now();
            response = cache_->get(req.key_, size);
        // std::cout << std::get<2>(req) << " [key: " << std::get<0>(req) << ", val: " << std::get<1>(req) <<"]"<< std::endl;
            t2 = std::chrono::high_resolution_clock::now();
            if(response != nullptr) {
                hits += 1;
            }
        } else if (req.method_ == "set") {
            t1 = std::chrono::high_resolution_clock::now();
            cache_->set(req.key_, req.val_, strlen(req.val_));
        // std::cout << std::get<2>(req) << " [key: " << std::get<0>(req) << ", val: " << std::get<1>(req) <<"]"<< std::endl;
            t2 = std::chrono::high_resolution_clock::now();
        } else if (req.method_ == "del") {
            t1 = std::chrono::high_resolution_clock::now();
            cache_->del(req.key_);
        // std::cout << std::get<2>(req) << " [key: " << std::get<0>(req) << ", val: " << std::get<1>(req) <<"]"<< std::endl;
            t2 = std::chrono::high_resolution_clock::now();
        }
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds> (t2-t1);
        results[i] = duration;
    }
    std::cout << "hit rate: " << hits / nreq << std::endl;
    return results;
}

std::pair<double, double> Driver::baseline_performance(int nreq) {
    std::vector<std::chrono::milliseconds> latencies = baseline_latencies(nreq);
    int index = PERCENTILE_METRIC * nreq / 100;
    std::sort(latencies.begin(), latencies.end());
    double percentile = latencies[index].count();
    double throughput = 0;
    return std::make_pair(percentile, throughput);
}
