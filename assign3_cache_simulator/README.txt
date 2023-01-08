Jianan Xu (jxu147@jhu.edu)
This was an individual work.

Part (b): Best cache
======================

I ran the experiment by the script `exp.py`, in which I considered various combinations of sets #, blocks #, 
block size, write policy, eviction policy. Here I list all cases I considered for each argument:
sets # = ['1', '4', '8', '16', '64', '128', '256']
blocks # = ['1', '2', '4', '8', '16', '64', '1024']
block size = ['4', '16', '64', '128']
write policy = ['write-allocate write-back', 'no-write-allocate write-through', 'write-allocate write-through']
evict policy = ['lru', 'fifo']
The experiment output can be found in the file 'exp_output.txt'

***NOTE: All by variable-controlling approach***
## Block size:
- hit rates & miss penalties & total cache size & time to fetch block:
- I found the larger the cache, the more likely that least recently accessed information remains in the cache. There
  is a higher cache-hit rate, less overhead, and less miss. On the contrary, the smaller the cache, the more likely 
  that least recently accessed information will be removed from the cache. There are more overhead, more misses, and 
  a lower cache-hit rate.

  Cache is fast because it's very small compared to the main memory and hence it requires small amount of time to 
  search it. A very large cache will not be able to perform at fast as the smaller counterpart.

  Thus, we need to compromise hit rate and speed. Genrally speaking, A cache hit ratio of 90% and higher means that 
  most of the requests are satisfied by the cache. From the experiment results, I found a chae with a block size of 
  64 Bytes is the best, which has a hit rate of 99.34% (2nd higher) and total cycles of 6190883 (2nd lower). (This is
  an example) 

## associativity:
- Similarly, we should balance hit rate and access time. Direct mapped caches have the poorest hit rate but the fastest
  access time. Fully associative caches have the best hit rates but the slowest access times because it has to compare 
  the address with every entry for accessing the data. So m-way set associative is ideal.
- From the experiment, 8-way set associative with 64 sets has a higher hit rate and also a satisfactory speed (fast).

## Write policy:
- Theoretically, write-back is almost always faster, because write-back reduces large eviction cost. Write-allocation
  works with both write-back and write-through. But write-allocate makes more sense for write-back because it is
  unnecessary to bring data from the memory to cache and then updating the data in both cache and main memory. 
- From the experiment results, the combination of write-allocate and write-back often has less total cycles than the 
  other two combinations. It is as expected.

## eviction policy:
- Theoretically, LRU keeps the things that were most recently used in memory. FIFO keeps the things that were most 
  recently added. LRU is, in general, more efficient.
- From the experiment results, LRU has a higher hit rate and lower total cycles than FIFO, but the advantage is not 
  so obvious in my experiment. LRU has a 0.007% higher hit rate and 2.5% lower total cycles than FIFO.

##### In coclusion, the best combination obtained from my experiment is 64 sets, 8-way set associative, 64-byte block,
      write-allocate, write-back, and LRU.


