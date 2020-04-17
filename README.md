Jon Takagi and Eli Poppele

## Benchmarking!

### Workload Generator
Our workload generator is implemented by the Generator class in `gen.hh` and `gen.cc`. This class has a single method, along with a constructor (a bit complicated) and a destructor (basic/empty implementation). The generator tries to create a similar workload to the ETC cache from the paper. It uses a struct, `Request`, as a return type to pass to the benchmark/driver program.

#### Constructor
The constructor for Generator sets a few parameters to control temporal locality and the amount of data produced, and then it fills the `data_` vector to create a workload. The `data_` vector contains tuples of a key (string), a size for the value that goes with the key (int), and a probability value (also int). The constructor then produces such tuples and adds them to the vector until the total size of the values in `data_` is at least equal to the `size` parameter times the `size_factor`. `size` in the constructor is meant to be the size of the target cache, while `size_factor` is a double (that should be greater than 1) to give the ratio of data in the cache versus total data--we want to fill the cache, and then have a fair amount of other values not in the cache in order to get an 80% hitrate.
To create a key, the constructor draws from a normal integer distribution with mean 43 and standard deviation 9; this is a simple distribution that models the key size distribution from the paper relatively well. The key is then created in the form "key_######f", where the ###### is a unique six-digit identifier for the key (we anticipate not needing more than 1 million keys for a cache of ~1 million bytes, even with a high `size_factor`).
