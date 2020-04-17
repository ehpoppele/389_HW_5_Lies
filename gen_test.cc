
#include <iostream>
#include "catch.hpp"
#include "cache/cache.hh" //Can probably remove this? once I get the subclass in its own thing
#include "cache/fifo_evictor.h"
#include "gen.hh"
#include <chrono>

int main() {
    auto gen = Generator(10, 0.5, 10000, 2);
    std::chrono::time_point<std::chrono::high_resolution_clock> t1;
    std::chrono::time_point<std::chrono::high_resolution_clock> t2;

    t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100; i++) {
        std::cout << i << "\t";
        Request req = gen.gen_req(true);
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds> (t2-t1);
    std::cout << duration.count() / 100 << "ms" << std::endl;

    // int sets = 0;
    // while(sets< 10) {
    //     Request req = gen.gen_req(false);
    //
    //     std::cout << ".";
    //     if(req.method_ == "set") {
    //         std::cout << req.method_ << " key: " << req.key_ << " to: " << req.val_ << std::endl;
    //         sets+= 1;
    //     }
    // }
    return 0;
}
