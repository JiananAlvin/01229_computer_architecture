#include "cache.h"
#include "utils.h"
#include "cacheset.h"

#define NEWEST_ACCESSED_LRU 0

Cache::Cache(int cache_size_input, int set_size_input, int block_size_input, bool write_allocate_flag_input, bool write_through_flag_input, bool lru_flag_input)
    : _cache_size(cache_size_input), _set_size(set_size_input), _block_size(block_size_input), _write_allocate_flag(write_allocate_flag_input), _write_through_flag(write_through_flag_input), _lru_flag(lru_flag_input), current_size(0)
{
    // init GlobalCounter
    GlobalCounter(counter);
    // calculate the bits of address
    // |  t bits  |  s bits   |      b bits        |
    // |   tag    | set index |   block offset     |
    // |find block|  find set | find data in block |
    _bit_num_index = intLog2(_cache_size);
    _bit_num_offset = intLog2(_block_size);
    _bit_num_tag = 32 - _bit_num_index - _bit_num_offset;
}

/*
 * Do cache load operation.
 *
 * Parameters:
 *   s -  a string of 32-bit memory address given in hexadecimal
 */
void Cache::doLoad(string s)
{
    uint32_t tag_value = getTagValue(s);
    uint32_t index_value = getSetIndexValue(s);
    uint32_t offset_value = getBlockOffsetValue(s);

    if (sets_of_cache.find(index_value) == sets_of_cache.end())
    {
        // load miss for sets
        addNewSet(offset_value, tag_value, index_value);
        // load from memory
        doLoadMiss();
        return;
    }

    // set exits, try to find correct block according to tag
    unordered_map<int, CacheSet>::iterator set_itr = sets_of_cache.find(index_value);
    CacheSet *current_set = &set_itr->second;
    pair<int, list<Block>::iterator> finding_result = current_set->find(tag_value);
    int finding_result_index = finding_result.first;

    if (finding_result_index == -1)
    {
        // load miss for blocks
        // if the set does not contain the tag id, and it's not full
        if (_lru_flag)
        {
            current_set->insertLRU(tag_value);
        }
        else
        {
            current_set->insertFIFO(tag_value);
        }
        doLoadMiss();
        return;
    }
    else
    {
        // if load hit
        list<Block>::iterator finding_result_itr = finding_result.second;
        if (_lru_flag)
        {
            current_set->updateLRU(finding_result_index, finding_result_itr);
        }
        doLoadHit();
        return;
    }
}

/*
 * Do cache store operation.
 *
 * Parameters:
 *   s -  a string of 32-bit memory address given in hexadecimal
 */
void Cache::doStore(string s)
{
    uint32_t tag_value = getTagValue(s);
    uint32_t index_value = getSetIndexValue(s);
    uint32_t offset_value = getBlockOffsetValue(s);
    Block *b;

    if (sets_of_cache.find(index_value) == sets_of_cache.end())
    {
        if (_write_allocate_flag == 0) // no-write-allocate
        {
            doStoreMiss(b);
            return;
        }
        // store miss for sets
        addNewSet(offset_value, tag_value, index_value);
        // store from memory
        // get current block
        unordered_map<int, CacheSet>::iterator set_itr = sets_of_cache.find(index_value);
        CacheSet *current_set = &set_itr->second;
        b = &*current_set->find(tag_value).second; // obtain a pointer from an iterator
        doStoreMiss(b);
        return;
    }

    // set exits, try to find correct block according to tag
    unordered_map<int, CacheSet>::iterator set_itr = sets_of_cache.find(index_value);
    CacheSet *current_set = &set_itr->second;
    pair<int, list<Block>::iterator> finding_result = current_set->find(tag_value);
    int finding_result_index = finding_result.first;

    if (finding_result_index == -1)
    {
        // store miss for blocks
        // if the set does not contain the tag id, and it's not full
        // for no-write-allocate a cache miss during a store does not modify the cache
        if (_write_allocate_flag == 1)
        {
            if (_lru_flag)
            {
                current_set->insertLRU(tag_value);
            }
            else
            {
                current_set->insertFIFO(tag_value);
            }
        }
        // get current block
        b = &*current_set->find(tag_value).second; // obtain a pointer from an iterator
        doStoreMiss(b);
    }
    else
    {
        // if store hit
        list<Block>::iterator finding_result_itr = finding_result.second;
        if (_lru_flag)
        {
            current_set->updateLRU(finding_result_index, finding_result_itr);
        }
        // get current block
        b = &*current_set->find(tag_value).second; // obtain a pointer from an iterator
        doStoreHit(b);
    }
}

/*
 * Print output information.
 */
void Cache::printConclusion()
{
    counter.printConclusion();
}

/*
 * Extract block tag.
 *
 * Parameters:
 *   s -  a string of 32-bit memory address given in hexadecimal
 *
 * Returns:
 *   a block tag
 */
uint32_t Cache::getTagValue(string s)
{
    // parse to int
    uint32_t value = stoul(s, nullptr, 16);
    return value >> (_bit_num_index + _bit_num_offset);
}

/*
 * Extract set index.
 *
 * Parameters:
 *   s -  a string of 32-bit memory address given in hexadecimal
 *
 * Returns:
 *   a set index
 */
uint32_t Cache::getSetIndexValue(string s)
{
    if (_bit_num_index == 0)
    {
        // depends on compiler, if a uint32_t >> 32, some compiler will not execute it.
        return 0;
    }
    uint32_t value = stoul(s, nullptr, 16);
    value = value << _bit_num_tag;
    return value >> (_bit_num_offset + _bit_num_tag);
}

/*
 * Extract offset.
 *
 * Parameters:
 *   s -  a string of 32-bit memory address given in hexadecimal
 *
 * Returns:
 *   an offset
 */
uint32_t Cache::getBlockOffsetValue(string s)
{
    uint32_t value = stoul(s, nullptr, 16);
    value = value << (_bit_num_tag + _bit_num_index);
    return value >> (_bit_num_tag + _bit_num_index);
}

/*
 * Update counter when load hit.
 */
void Cache::doLoadHit()
{
    counter.loadHitIncre();
    counter.cacheOperationCycleIncre();
}

/*
 * Update counter when load miss.
 */
void Cache::doLoadMiss()
{
    counter.loadMissIncre();
    counter.cacheOperationCycleIncre();
    counter.memoryOperationCycleIncre(_block_size);
}

/*
 * Update counter when store hit.
 *
 * Parameters:
 *   b - pointer to current block
 */
void Cache::doStoreHit(Block *b)
{
    // incre hit
    counter.storeHitIncre();
    if (_write_through_flag)
    {
        // write-through, CPU -> cache
        counter.cacheOperationCycleIncre();
        // cache -> memory
        counter.cycle_count += 100;
    }
    else
    {
        // write-back, CPU -> cache
        counter.cacheOperationCycleIncre();
        // make current block dirty
        b->dirty = 1;
    }
}

/*
 * Update counter when store miss.
 *
 * Parameters:
 *   b - pointer to current block
 */
void Cache::doStoreMiss(Block *b)
{
    // write allocate: memory -> load into cache
    // for write-allocate we bring the relevant memory block into the cache before the store proceeds
    // for no-write-allocate a cache miss during a store does not modify the cache
    counter.storeMissIncre();

    if (_write_allocate_flag)
    {
        // bring the relevant memory block into the cache before the store proceeds
        // memory -> cache
        counter.memoryOperationCycleIncre(_block_size);
        // Dirty eviction has been checks in insertItem().
        counter.cacheOperationCycleIncre(); // CPU -> cache
        // make current block dirty
        b->dirty = 1;
    }
    else
    {
        // No-write-allocate, CPU -> memory
        counter.cycle_count += 100;
    }
}

/*
 * Add a new set to cache.
 *
 * Parameters:
 *   offset_value - offset in the input address
 *   tag_value    - tag in the input address
 *   index_value  - set index in the input address
 */
void Cache::addNewSet(int offset_value, int tag_value, int index_value)
{
    Block *new_block = new Block(tag_value, _block_size);
    CacheSet *new_set = new CacheSet(_set_size, _block_size, &counter, _write_through_flag);

    new_set->current_size = 1; // Only one block in this set now.
    new_set->blocks_of_set.push_back(*new_block);
    sets_of_cache.insert(make_pair(index_value, *new_set));

    current_size++; // containing more sets
}
