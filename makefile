
CXX=g++-8
CXXFLAGS=-Wall -Wextra -pedantic -Werror -std=c++17 -O0 -g -I /usr/local/boost_1_72_0/ -pthread
LDFLAGS=$(CXXFLAGS)
OBJ=$(SRC:.cc=.o)
BUILDDIR=out/
VPATH=cache/

all: server.bin driver_test.bin gen_test.bin

test.bin: cache_lib.o test_cache_lib.o fifo_evictor.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) -c $< -o $@

clean:
	rm *.o
	rm *.bin

server.bin: cache_lib.o fifo_evictor.o tcp_listener.o udp_handler.o
	$(CXX) $(LDFLAGS) cache/cache_server.cc -o $@ $^ /vagrant/systems/boost/lib/libboost_program_options.a

driver_test.bin: cache_client.o fifo_evictor.o driver.o gen.o
	$(CXX) $(LDFLAGS) driver_test.cc -o $@ $^
gen_test.bin: gen.o
	$(CXX) $(LDFLAGS) gen_test.cc -o $@ $^
