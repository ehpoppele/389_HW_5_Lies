#include "cache.hh"
#include "gen.hh"
#include <chrono>

//The driver class creates a networked cache, then runs commands on it
//It can call set, get, or delete, and the params can be adjusted to emulate the ETC workload
class Driver {

    public:

    private:
        Cache* cache_;
        Generator gen_;

    public:
        Driver(Cache* cache, Generator gen, int temporal_bias = 10);

        ~Driver();

        // Disallow driver copies, since cache copies are disallowed and driver has a unique_ptr to a cache.
        Driver(const Driver&) = delete;
        Driver& operator=(const Driver&) = delete;

        //Requests a single set from the cache
        void set_request(key_type key, Cache::val_type val, Cache::size_type size);

        //Requests a single get from the cache; we don't need the size back here since we mostly don't care about the value being returned
        Cache::val_type get_request(key_type key);

        //Requests a single delete from the cache
        bool del_request(key_type key);

        //Gets the total space used from the cache; used for testing the warm function
        int head_request();

        //Warms the cache by adding new kv pairs of the given total size
        void warm(int size);

        //Delete driver data and reset cache as well
        void reset();

        std::vector<std::chrono::milliseconds> baseline_latencies(int nreq);

        std::pair<double, double> baseline_performance(int nreq);

};
