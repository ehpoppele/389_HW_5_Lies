#define CATCH_CONFIG_MAIN
#include <iostream>
#include "catch.hpp"
#include "cache.hh" //Can probably remove this? once I get the subclass in its own thing
#include "fifo_evictor.h"
#include "gen.hh"

auto gen = Generator(10, 0.5, 10000, 2);

//Tests to ensure the driver has a connection to the server
//And that all types of basic requests are actually working and getting a response


TEST_CASE("Gen Request")
{
    //In this case, the results are printed, and the human tester
    //should observe a mix of get/set/delete requests with appropriate
    //results being processed
    SECTION("Print Test"){
        std::cout  << "Look over the next 20 lines and check for any unexpected behavior:" << std::endl;
        for(int i = 0; i < 20; i++){
            gen.gen_req(true);//true here indicates that responses should be printed
        }
        REQUIRE(true);
    }
}


/*
TEST_CASE("hitrate")
{
    SECTION("Hitrate 1"){
        int hits = 0;
        int gets = 0;
        while (gets < 1000000) {
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
                    std::cout << "not found" << std::endl;

                }
            }
        }
        REQUIRE(hits > 800000);
    }

    driver.reset();
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
