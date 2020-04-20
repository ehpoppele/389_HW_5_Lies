#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "driver.hh"
#include <iostream>
#include <fstream>
#include "cache/fifo_evictor.h"

Generator gen = Generator(8, 0.2, 8192, 8);
//Generator gen = Generator(120, 0.3, 8192, 120);
//auto test_cache = Cache("127.0.0.1", "42069"); //Add the appropriate params here once chosen
FifoEvictor fifo_evictor = FifoEvictor();
Evictor* evictor = &fifo_evictor;
auto test_cache = Cache(8192, 0.75, evictor); //Add the appropriate params here once chosen


const int trials = 10000;
//auto driver = driver();
Cache::size_type size;
auto driver = Driver(&test_cache, gen);

//Tests to ensure the driver has a connection to the server/cache
//And that all types of basic requests are actually working and getting a response
// TEST_CASE("Connection")
// {
//
//     SECTION("SET/GET"){//can't test set without using get
//         driver.set_request("key_one", "value_1", 8);
//         Cache::val_type val = "value_2";
//         REQUIRE(*driver.get_request("key_one") == *val);
//     }
//
//     SECTION("DELETE"){
//         REQUIRE(driver.del_request("key_one") == true);
//         REQUIRE(driver.get_request("key_one") == nullptr);
//     }
//
//     driver.reset();//resets cache, hitrate, and ... ?
// }


TEST_CASE("warm")
{
    SECTION("Warm"){//adds new values to cache summing to given size
        driver.warm(CACHE_SIZE);
        REQUIRE(driver.head_request() > 0.9 * size);//fix this
    }

    driver.reset();
}



TEST_CASE("Hitrate")
{

    SECTION("Hitrate at ~80%"){
        const int trials = 1000;
        int hits = 0;
        for(int i = 0; i < 100; i++){
            driver.reset();
            driver.warm(CACHE_SIZE);
            int gets = 0;
            while (gets < trials) {
                auto req = gen.gen_req(false);
                std::string method = req.method_;
                if(method == "get") {
                    gets += 1;
                    Cache::val_type res = driver.get_request(req.key_);
                    if(res != nullptr){
                        hits += 1;
                    }
                }
            }
        }
        std::cout<< "hits : " + std::to_string(hits) << " out of " + std::to_string(trials*10) << std::endl;
        REQUIRE(hits > trials * 100 * 0.75);
        REQUIRE(hits < trials * 100 * 0.85);
    }

    driver.reset();
}
TEST_CASE("prep_data") {
    SECTION("graph") {
        driver.warm(CACHE_SIZE);
        auto latencies = driver.baseline_latencies(trials);
        std::sort(latencies.begin(), latencies.end());
        std::ofstream output;
        output.open("latencies.dat");
        output << "latency (ms)" << std::endl;
        for(int i = 0; i < trials; i++) {
            output << latencies[i] << std::endl;
        }
        output.close();
    }
    driver.reset();
}

TEST_CASE("performance") {
    SECTION("part a") {
        driver.warm(CACHE_SIZE);
        auto results = driver.baseline_performance(trials);
        std::cout << "95th percentile latency: " << results.first << std::endl;
        std::cout << "mean throughput: " << results.second << std::endl;
    }
    driver.reset();
}
/*
//new driver here; use appropriate params
TEST_CASE("80% Hitrate")
{

    SECTION("Hitrate 80%"){
        driver.warm(size);
        driver.gen_req(1000);
        REQUIRE(0.77 < driver.hitrate());//We want 80% but i'm leaving some margin of error; may reduce that later
        REQUIRE(driver.hitrate() < 0.83);
    }
}
*/

//Want to test:
//ensure a connection/getting server responses
//test returning the cache size
//warm the cache: call the warm function and test with cache size
//get a hitrate and test its correctness






//In compisition of driver:
//20/30 get
//1/30 set
//9/30 delete
//Key Size:
//100% in ~half bell curve from 20 to 70 bytes
//Value size:
// 90% roughly linear up to 500B
//last 10% linear up to ~5000B
//Key repetition:
//first 50% of keys in >1% of requests
//next 40% in 9% or requests
//next 10% in 90% of requests
