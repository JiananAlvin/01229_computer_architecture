#include <iostream>
#include "globalcounter.h"

GlobalCounter::GlobalCounter()
{
    load_hit_count = 0;
    load_miss_count = 0;
    store_hit_count = 0;
    store_miss_count = 0;
    cycle_count = 0;
}

/*
 * Increment load hit counter.
 */
void GlobalCounter::loadHitIncre()
{
    GlobalCounter::load_hit_count++;
}

/*
 * Increment load miss counter.
 */
void GlobalCounter::loadMissIncre()
{
    GlobalCounter::load_miss_count++;
}

/*
 * Increment store hit counter.
 */
void GlobalCounter::storeHitIncre()
{
    GlobalCounter::store_hit_count++;
}

/*
 * Increment load miss counter.
 */
void GlobalCounter::storeMissIncre()
{
    GlobalCounter::store_miss_count++;
}

/*
 * Increment total cycles counter.
 */
void GlobalCounter::cycleIncre()
{
    GlobalCounter::cycle_count++;
}

/*
 * Increment total cycles counter when loads/stores from/to the cache.
 * That takes one processor cycle.
 */
void GlobalCounter::cacheOperationCycleIncre()
{
    GlobalCounter::cycle_count++;
}

/*
 * Increment total cycles counter when loads/stores from/to memory.
 * That takes 100 processor cycles for each 4-byte quantity that is transferred.
 */
void GlobalCounter::memoryOperationCycleIncre(int bit_num_block)
{
    GlobalCounter::cycle_count += bit_num_block * 25;
}

/*
 * Print output information.
 */
void GlobalCounter::printConclusion()
{
    std::cout << std::dec << std::endl;
    std::cout << "Total loads: " << load_hit_count + load_miss_count << std::endl;
    std::cout << "Total stores: " << store_hit_count + store_miss_count << std::endl;
    std::cout << "Load hits: " << load_hit_count << std::endl;
    std::cout << "Load misses: " << load_miss_count << std::endl;
    std::cout << "Store hits: " << store_hit_count << std::endl;
    std::cout << "Store misses: " << store_miss_count << std::endl;
    std::cout << "Total cycles: " << cycle_count << std::endl;
}
