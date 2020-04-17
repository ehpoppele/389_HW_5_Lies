#define CATCH_CONFIG_MAIN
#include <iostream>
#include "catch.hpp"
#include "cache.hh" //Can probably remove this? once I get the subclass in its own thing
#include "fifo_evictor.h"
#include "driver.hh"

Generator gen = Generator(10, 0.5, 10000, 2);
auto test_cache = Cache("127.0.0.1", "42069"); //Add the appropriate params here once chosen
//auto driver = driver();
Cache::size_type size;
auto driver = Driver(&test_cache, gen, 10);

//Tests to ensure the driver has a connection to the server
//And that all types of basic requests are actually working and getting a response
TEST_CASE("Connection")
{

    SECTION("SET/GET"){//can't test set without using get
        driver.set_request("key_one", "value_1", 8);
        Cache::val_type val = "value_2";
        REQUIRE(*driver.get_request("key_one") == *val);
    }

    SECTION("DELETE"){
        REQUIRE(driver.del_request("key_one") == true);
        REQUIRE(driver.get_request("key_one") == nullptr);
    }

    driver.reset();//resets cache, hitrate, and ... ?
}

TEST_CASE("Cache Warming")
{
    SECTION("HEAD"){
        driver.set_request("key_one", "value_one", 10);
        REQUIRE(driver.head_request() == 10);
    }

    SECTION("Warm"){//adds new values to cache summing to given size
        driver.warm(30);
        REQUIRE(driver.head_request() <= 40);//fix this
    }

    driver.reset();
}


TEST_CASE("hitrate")
{
    SECTION("Hitrate 1"){
        driver.warm(1024);
        const int trials = 100;
        int hits = 0;
        int gets = 0;
        while (gets < trials) {
            auto req = gen.gen_req(false);
            // Cache::val_type val = std::get<1>(req);
            std::string method = std::get<2>(req);
            if(method == "get") {
                key_type key = std::get<0>(req);
                std::cout << "key: " << key;
                gets += 1;
                Cache::val_type res = driver.get_request(key);
                if(res != nullptr){
                    std::cout << std::endl;
                    hits += 1;
                } else {
                    std::cout << "\t not found" << std::endl;
                }
            }
        }
        REQUIRE(hits > trials * 0.8);
    }

    driver.reset();
}

TEST_CASE("Gen Request")
{
    //In this case, the results are printed, and the human tester
    //should observe a mix of get/set/delete requests with appropriate
    //results being processed
    SECTION("Print Test"){
        driver.warm(30);
        std::cout  << "Look over the next 10 lines and check for any unexpected behavior:" << std::endl;
        for(int i = 0; i < 10; i++){
            gen.gen_req(true);//true here indicates that responses should be printed
        }
    }
    //no reset since this is the end of the current driver
}

TEST_CASE("performance") {
    SECTION("part a") {
        driver.warm(30);
        driver.baseline_performance(1000000);
    }
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
