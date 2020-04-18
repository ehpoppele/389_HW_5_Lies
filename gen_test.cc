#include <iostream>
#include "gen.hh"
#include <chrono>
#include <cstring>

int main() {
    auto gen = Generator(120, 0.3, 8192, 120);
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
    int total = 0;
    while(total< 100000) {
        Request req = gen.gen_req(false);
        if(req.method_ == "set") {
            if(req.key_ == "") {
                std::cout << "Failed on setting empty key, at " << total << "with " << req.key_ << std::endl;
                return -1;
            }
            /*
            if(strlen(req.val_) == 0) {
                std::cout << "failed on setting empty value, at " << total<< "with " << req.key_ << std::endl;
                return -1;
            }
            */
        }
        if(req.method_ == "get") {
            if(req.key_ == "") {
                std::cout << "failed on getting empty key, at " << total << "with " << req.key_ << std::endl;
                return -1;
            }
        }
        total++;
    }
    std::cout << zero_length_keys << " of " << trials << " keys tested had length 0 (" << zero_length_keys / sets << "%)" << std::endl;
    return 0;
}
