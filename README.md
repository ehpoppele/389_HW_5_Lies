Jon Takagi and Eli Poppele

## Benchmarking!

### Workload Generator
Our workload generator is implemented by the Generator class in `gen.hh` and `gen.cc`. This class has a single method, along with a constructor (a bit complicated) and a destructor (basic/empty implementation). The generator tries to create a similar workload to the ETC cache from the paper. It uses a struct, `Request`, as a return type to pass to the benchmark/driver program.

#### Constructor
The constructor for Generator sets a few parameters to control temporal locality and the amount of data produced, and then it fills the `data_` vector to create a workload. The `data_` vector contains tuples of a key (string), a size for the value that goes with the key (int), and a probability value (also int). The constructor then produces such tuples and adds them to the vector until the total size of the values in `data_` is at least equal to the `size` parameter times the `size_factor`. `size` in the constructor is meant to be the size of the target cache, while `size_factor` is a double (that should be greater than 1) to give the ratio of data in the cache versus total data--we want to fill the cache, and then have a fair amount of other values not in the cache in order to get an 80% hitrate. Below, I describe how we produce such data tuples, although this is probably not necessary to know unless our code looks quite off.

To create a key, the constructor draws from a normal integer distribution with mean 43 and standard deviation 9; this is a simple distribution that models the key size distribution from the paper relatively well. The key is then created in the form "key_######f", where the ###### is a unique six-digit identifier for the key (we anticipate not needing more than 1 million keys for a cache of ~1 million bytes, even with a high `size_factor`). The character 'f' is repeated at then end of the key as filler until the target length is achieved. Since all keys are at least 15 characters, they will always have some 'f's on their tail end.

To create a value, we have used three distributions to approximate what is shown in the paper. This created a value size distribution that is at least pretty close to the given one. We first select a percent value (0-100) from a uniform integer distributuion. Then, working from the second graph in figure 2, we use a low distribution for the first 40%, medium for the next 50%, and high for the last 10%. As both low and high appear as roughly linear on the logarithmic CDF graph, this means they have a CDF following an exponential distribution. Since our attempts to use the boost distributions failed, we approximate an exponential distribution by taking the absolute value of normal distributions (effectively half of a bell curve, with double the height at each point). The low distribution starts at 2 and tails down to 10 bytes with low probability (~3 standard deviations, and the value is clamped at this maximum), while the high distribution starts at 500 and similarly tails off to 5000 bytes. The medium distribution appears exponential on the logarithmic CDF graph, so it seems reasonable that its normal CDF graph is linear, and thus it is modeled as a normal distribution of integers from 10 to 500. Since the actual content of values does not matter, we simply generate and store this size value, and later create an actual value by repeating the character "B" an appropriate number of times.

Finally, to select a probability, we generate another percentage and run it through the function `prob = (int)(1000)/((p+1)^2.3)`. This function then returns high probability (1000) for the top 1% of keys, low probability (4) for the bottom 90% of keys, and very unlikely probability (0.1%) for the bottom 50% of keys. This models the given key distribution from the paper, where the top 1% of keys made up 50% of requests, the top 10% made up 90% of requests, and the top 50% made up more than 99% of requests. If you sum the values generated by this function over one of those given distributions, they make up a similar percentage of the sum over all values (0-100) of the function. The probability given to a key is directly correlated to how likely it is to be used in a request. A key with probability 500 is then 50 times more likely to be used than a key with probability 10. The implementation for this selection appears in the `gen_req` function. 

The `data_` vector is then shuffled at the end of the constructor, as position in the vector will imply temporal locality and we want this to be random.

#### Request Generation
To generate a new request from our workload, we use the `gen_req` function. This selects a tuple from the `data_` vector, taking into account temporal locality and the probabilities of each data tuple, and then returns it as a `Request` object. This struct is defined in `request.hh` and has a very simple implementation; it simply hold a key, a value size, and a method. This is then returned to the driver which called the `gen_req` function.

To add temporal locality to our data, we have use two parameters and keep track of one location. The parameters are `locality_range_` and `locality_shift_`. `locality_range_` is an integer which determines what fraction of the `data_` vector we work with at once; if the value is 100, then each time we generate a request we are only using some sequential block of values that makes up 1% of the data, and we are selecting randomly from that block. `locality_shift_` then tells us how much we shift this sequential block by each time that we call `gen_req`. If it is 0.3, then we advance our sequential block by 0.3 times its length for each `gen_req` call, and we implemented a wrap-around in the data vector so that if we were to extend pass the end in this manner, we go back to the beginning to use for our block. The position of this sequential block is track with `current_locality_`, which tracks how many shifts in we are when generating a request. For example, with the same paramaters as above, if we had a `current_locality_` of 10 at the start of our `gen_req` function, we would be selecting randomly from the 4th 1% of values; that is, from the tuples that make up the 3rd to 4th percentile of our data vector. The `current_locality_` is then increased at the end of the function, and is reset to zero for wrap-around if it would exceed its maximum value.

To actually select a tuple at random, we select some target probability, between zero and `total_prob_/locality_range_`. While this doesn't necessarily match up with the actual total of probabilities over our current locality range, it is statistically close. We then iterate over the tuples in our current range, adding their probability to a running sum. Once some probability causes our sum to exceed or equal the target, we take that tuple and use it to return a request. In this way, we give proportionally higher chances to tuples that have a higher probability value. Once we have selected a tuple, we return it's value size and key, along with a randomly selected method (using a simple ratio of 20 gets : 9 deletes : 1 set). However, if we are returning a delete, we will 8 out of 9 times have it use a non-existent key, so that we can maintain a roughly full cache throughout our `gen_req` calls.

#### Hitrate
With a cache size of 8192, we found that a `locality_range_` of 8, a `locality_shift_` of 0.2, and a `size_factor_` of 8 got us close to an 80% hitrate. We test this in `driver_test.cc`, first warming the cache and then making one thousand get requests. The hitrate is highly variable for each new cache and the results of warming it; the reasons for this are unclear, but we tried to work around this by taking the average of 100 caches, cleared and warmed each time. This returns a hitrate of about the correct range.

### Driver
We have implemented our benchmark program in the driver files, `driver.hh` and `driver.cc`. This class takes a generator and a cache, and applies the requests generated to the cache. It has three main functions: `warm`, `baseline_latencies`, and `baseline_performance`.

`warm` simply makes a given number of set requests to the cache. We find that using a number close to or greater than the size of the cache usually works to fill it; while most values set are much larger than 1 bytes, most are also called many times in one instance of `warm`, so we need many calls to get the cache close to full. 

`baseline_latencies` works essentially as described in the project writeup, getting one request from the generator, then timing the latency of that request to the cache and recording it in the vector. `baseline_performance` similarly works as described.

Using our standard parameters (8KB of memory on a networked cache), we had a 95th percentile latency of 0.275198ms and a mean throughput of 693481 requests/second. The latency graph for these data is shown below.
![Standard Latency](/latency&#32;8KB&#32;networked.png)

### Sensitivity Testing
For testing, we chose to test underlying architecture, local clients, maximum memory size, and maximum load factor. We present the data for these changes in the same graph format as that of the standard test above. 

For underlying architecture, we ran the networked 8KB test on a different computer, running linux natively as opposed to in a virtual machine. In this case, we could only test this one alternative, so the result is shown in the graph below. Although this second machine is generally slower (we believe) it seems to have an advantage in a native installation as opposed to a virtual machine.
![Native Linux Latency](/Eli_latency.png)

For local versus networked client, we again had only one option, and so the latency results of running with a local cache instead of a networked cache appear below, and show a significant decrease in latency and an increase in consistency, as expected.
![Local Latency](/latency&#32;8KB&#32;local.png)

Using my (Eli's) computer, I got: 95th percentile latency: 0.071494ms ; mean throughput: 3.33333e+07req/s.


