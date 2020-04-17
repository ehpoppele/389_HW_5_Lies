#include "gen.hh"

int main() {
    auto gen = Generator(10, 0.5, 100000, 2);
    for(int i = 0; i < 100; i++){
        gen.gen_req(true);
    }
    return 0;
}
