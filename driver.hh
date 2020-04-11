#include "cache.hh"

//The driver class creates a networked cache, then runs commands on it
//It can call set, get, or delete, and the params can be adjusted to emulate the ETC workload
class Driver {

    private:
        std::unique_ptr<Cache> cache_;
        //data vector
        //key vector
        int delete_bias_; //since we delete 9 times more often than we set, something must be done to keep the cache from depleting; this var sets a bias on delete to delete items that are less likely to actually be in the cache
        double hits_; //tracks cache hits; updated on each result
        double misses_; //tracks the cache misses; updated on each result

    public:
        Driver(bool print_results, //for testing purposes; will print the requests and reponses in gen_req
               Cache cache,
               int delete_bias);

        ~Cache();

        // Disallow driver copies, since cache copies are disallowed and driver has a unique_ptr to a cache.
        Driver(const Driver&) = delete;
        Driver& operator=(const Driver&) = delete;

        //Requests a single set from the cache
        void set_request(key_type key, val_type val, size_type size);

        //Requests a single get from the cache; we don't need the size back here since we mostly don't care about the value being returned
        val_type get_request(key_type key);

        //Requests a single delete from the cache
        bool del_request(key_type key);

        //Gets the total space used from the cache; used for testing the warm function
        int head_request();

        //Warms the cache by adding new kv pairs of the given total size
        void warm(int size);

        //Generates n new requests to the cache; the size, frequency, values, etc. are chosen at random based on the distributions in the driver's private data
        void gen_req(int n);

        //returns the hitrate of the cache based on the hit/miss data; will have to do the division itself to save having to do that every time a new request either hits or misses
        double hitrate();

        //Delete driver data and reset cache as well
        void reset();
        
};
