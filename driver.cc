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

const int PERCENTILE_METRIC = 95;
//The constructor doesn't do much, since we leave most of the heavy lifting on distribution generation etc to the warm method
Driver::Driver(Cache* cache, int temporal_bias)
{
    temporal_bias_ = temporal_bias;
    cache_ = cache;
    total_prob_ = 0;
}

Driver::~Driver()
{
    //reset the cache? I forgot what this does
}

//warm generates new data and adds to vector until cache has gotten at least size total data
//It creates tuples of key, val, size, and probability that are drawn from distributions designed to mimic the ETC workload
void Driver::warm(int size)
{
    total_prob_ = 0;
    data_.clear();
    int size_used = 0;
    int i = 0;
    //Now we set up the randomization, and the different distributions
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::normal_distribution<double> key_dist(43.0, 9.0);//want a normal dist spanning 15-70, so use this and clamp later
    std::uniform_int_distribution<int> percent_dist(0, 100);
    //std::normal_distribution<double> val_dist_low(1.0, 3.0);
    //std::uniform_int_distribution<int> val_dist_mid(10, 500);
    //std::normal_distribution<int> val_dist_high(0.0, 1500.0);
    boost::mt19937 randGen(15);
    boost::math::pareto_distribution val_dist(214.476, 0.348238);
    boost::random::uniform_real_distribution<> uniformReal(1.0,5000.0); //this range can be adjusted to effect values
    boost::variate_generator<boost::mt19937&,boost::random::uniform_real_distribution<>> generator(randGen, uniformReal);
    while(size_used < 2*size){//we want the cache full and still plenty of pairs left; pointless to test if cache can hold 100% of data
        //First generate a new data tuple based on our distributions
        int key_size = std::clamp((int)key_dist(rng), 15, 70); //should be normal dist between 15 and 70
        int padding = (int)(log10(1000000 - i));//since cache has max memory of ~1mil bytes, should never need more than that many keys
        int filler = key_size - 10; //10 chars needed for "key_000000"; then the rest is filler "f"
        key_type key = "key_" + std::string(padding, '0') + std::to_string(i) + std::string(filler, 'f');//f for filler and f to pay respects to all those wasted bytes
        //int val_size;//approximated with 3 distributions, so we select one first
        /*
        int p = percent_dist(rng);
        if(p < 40){
            val_size = std::clamp(abs(val_dist_low(rng)), 1, 10);//use the normal dist, but want only positive nums and no greater than 10
        } else if(p < 90){
            val_size = val_dist_mid(rng);
        } else {
            val_size = std::clamp(abs(val_dist_high(rng)), 0, 4500) + 500;//same as low vals, but need to boost by 500 since dist is centered at 0
        }
        */
        //with thanks to MS_DDOS on stack exchange for being seemingly the only person to actually answer a question about boost pareto use, even if it was their own question
        double cdfComplement;
        cdfComplement = boost::math::cdf(complement(val_dist,generator()));
        int val_size = std::clamp((int)cdfComplement, 1, 5000);//convert?
        std::string val_str = std::string(val_size, 'B');//the values for entries are just the character "B" repeated up to 5000 times
        Cache::val_type val = val_str.c_str();
        size_used += val_size;
        int prob;
        int p = percent_dist(rng);
        prob = (int)(1000)/(p+1)^2;
        total_prob_+= prob;
        data_type data_tuple = std::make_tuple(key, val_size, prob);
        cache_->set(key, val, size);
        data_.push_back(data_tuple);
        i++;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(data_.begin(), data_.end(), std::default_random_engine(seed));
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

std::tuple<key_type, Cache::val_type, std::string> Driver::gen_req(bool print_results)
{
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::normal_distribution<double> key_dist(43.0, 9.0);//want a normal dist spanning 15-70, so use this and clamp later
    std::uniform_int_distribution<int> method_dist(0, 30);
    std::string method;
    int p = method_dist(rng);
    if(p < 21){
        method = "get";
    } else if (p < 30){
        method = "del";
        if(method_dist(rng) < 25){//8/9 of deletes should delete a nonexistent key to keep the cache full
            int dummy_key_length = std::clamp((int)key_dist(rng), 15, 70);
            if(print_results){
                std::cout << std::string(dummy_key_length, 'f') + ", "<< 1 + ", "<< method << std::endl;
            }
            return std::make_tuple(std::string(dummy_key_length, 'f'), "B", method);//this is not a key that will ever be used for "set"
        }
    } else {
        method = "set";
    }
    //now we select a key/value pair at random; the readme will describe how this process works
    std::uniform_int_distribution<int> kv_dist(0, total_prob_/temporal_bias_);
    int target = kv_dist(rng);
    int current = 0;
    int i = 0;
    data_type kv_tuple;
    while(current + std::get<2>(data_[i]) < target){//getting the probability of the current data_tuple in the vector
        //iterate through
        current += std::get<2>(data_[i]);
        i+=1;

    }
    key_type key = std::get<0>(kv_tuple);
    std::string val_str = std::string(std::get<1>(kv_tuple), 'B');
    Cache::val_type val = val_str.c_str();
    if(print_results){
        std::cout << key << std::get<1>(kv_tuple) + ", " << method + ", "<< std::endl;
    }
    return std::make_tuple(key, val, method);
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
        req_type req = gen_req(false);
        key_type key = std::get<0>(req);
        Cache::val_type val = std::get<1>(req);
        std::string method = std::get<2>(req);
        Cache::size_type size = 0;
        if(method=="get") {
            Cache::val_type response;
            t1 = std::chrono::high_resolution_clock::now();
            response = cache_->get(key, size);
        // std::cout << std::get<2>(req) << " [key: " << std::get<0>(req) << ", val: " << std::get<1>(req) <<"]"<< std::endl;
            t2 = std::chrono::high_resolution_clock::now();
            if(response != nullptr) {
                hits += 1;
            }
        } else if (method == "set") {
            t1 = std::chrono::high_resolution_clock::now();
            cache_->set(key, val, size);
        // std::cout << std::get<2>(req) << " [key: " << std::get<0>(req) << ", val: " << std::get<1>(req) <<"]"<< std::endl;
            t2 = std::chrono::high_resolution_clock::now();
        } else if (method == "del") {
            t1 = std::chrono::high_resolution_clock::now();
            cache_->del(key);
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
