#include "cache.hh"

//The driver class creates a networked cache, then runs commands on it
//It can call set, get, or delete, and the params can be adjusted to emulate the ETC workload
class Driver {

    public:
        using data_type = std::tuple<key_type, int, int>; //holds key, value size, and probability

    private:
        Cache* cache_;
        std::vector<data_type> data_;//data vector, which holds all the kv pairs and their data that will be used for this driver
        std::vector<key_type> keys_;//key vector, which keeps track of the keys that have actually been used so far; i don't remember why we need this...
        int total_prob_; //tracks the sum total of the probability entries in the data vector, as this is fixed after one warm call
        int temporal_bias_;

    public:
        Driver(Cache* cache, int temporal_bias = 10);

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

        //Generates a new request for the cache; the size, frequency, values, etc. are chosen at random based on the distributions in the driver's private data
        //NOTE: you MUST first warm the cache before using gen_req, since warming will create the appropriate data vectors
        //gen_req relies on those vectors and will segfault if they are empty (which is their default)
        std::tuple<std::string, std::string, std::string> gen_req(bool print_results=false);

        //Delete driver data and reset cache as well
        void reset();

        std::vector<double> baseline_latencies(int nreq);

};
