
#include <iostream>
#include "catch.hpp"
#include "cache.hh" //Can probably remove this? once I get the subclass in its own thing
#include "fifo_evictor.h"
#include "gen.hh"

int main() {
    auto gen = Generator(10, 0.5, 10000, 2);
    //print
    return 0;
}
