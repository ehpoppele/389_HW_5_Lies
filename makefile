
CXX=g++-8
CXXFLAGS=-Wall -Wextra -pedantic -Werror -std=c++17 -O3 -g -I /usr/local/boost_1_72_0/ -pthread
LDFLAGS=$(CXXFLAGS)
OBJ=$(SRC:.cc=.o)
BUILDDIR=out/
VPATH=cache/

all: server.bin test_driver.bin test_gen.bin

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c $< -o $@

clean:
	rm *.o
	rm *.bin

server.bin: cache_lib.o fifo_evictor.o tcp_listener.o udp_handler.o
	$(CXX) $(LDFLAGS) cache/cache_server.cc -o $@ $^ /usr/local/boost_1_72_0/ehpop/Documents/lib/libboost_program_options.a

test_driver.bin: fifo_evictor.o driver.o gen.o cache_client.o
	$(CXX) $(LDFLAGS) driver_test.cc -o $@ $^
test_gen.bin: gen.o
	$(CXX) $(LDFLAGS) gen_test.cc -o $@ $^
