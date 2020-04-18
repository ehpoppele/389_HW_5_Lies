

#include <iostream>
#include "catch.hpp"
#include "cache/cache.hh" //Can probably remove this? once I get the subclass in its own thing
#include "cache/fifo_evictor.h"
#include "gen.hh"
#include <chrono>
#include <cstring>

int main() {
    auto gen = Generator(10, 0.5, 10000, 2);
    // std::chrono::time_point<std::chrono::high_resolution_clock> t1;
    // std::chrono::time_point<std::chrono::high_resolution_clock> t2;
    //
    // t1 = std::chrono::high_resolution_clock::now();
    // for(int i = 0; i < 100; i++) {
    //     std::cout << i << "\t";
    //     Request req = gen.gen_req(true);
    // }
    // t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds> (t2-t1);
    // std::cout << duration.count()<< "ms" << std::endl;

    int sets = 0;
    while(sets< 100000) {
        Request req = gen.gen_req(false);
        // std::cout << ".";
        if(req.method_ == "set") {
            if(req.key_ == "") {
                std::cout << "failed on set " << sets << std::endl;
                return -1;
            }
            // std::cout << "\n" << req.method_ << " key: " << req.key_ << " to: " << req.val_ << std::endl;
            sets+= 1;
        }
    }
    std::cout << "passed" << std::endl;
    return 0;
}
