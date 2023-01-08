#include "cacheset.h"
#include <iostream>
#include <algorithm>

CacheSet::CacheSet(int set_size_input, int block_size_input, GlobalCounter *counter_input, bool write_through_flag_input)
    : set_size(set_size_input), block_size(block_size_input), current_size(0), counter(counter_input), write_through_flag(write_through_flag_input){};

/*
 * Insert a block into blocks list. If the list is not full put the block
 * in front of the list. Otherwise, envict a block.
 *
 * Parameters:
 *   tag_value    - tag value of incoming block
 */
void CacheSet::insertItem(int tag_value)
{

    if (current_size < set_size)
    {
        // not full
        // insert a new one to head
        Block *new_block = new Block(tag_value, block_size);
        blocks_of_set.push_front(*new_block);
        current_size++;
    }
    else
    {
        // if FULL, evict
        // remove the tail, add a new one to head
        Block *new_block = new Block(tag_value, block_size);
        evict(&blocks_of_set);
        blocks_of_set.push_front(*new_block);
    }
}

/*
 * Insertion for LRU policy.
 *
 * Parameters:
 *   tag_value    - tag value of incoming block
 */
void CacheSet::insertLRU(int tag_value)
{
    insertItem(tag_value);
}

/*
 * Insertion for FIFO policy.
 *
 * Parameters:
 *   tag_value    - tag value of incoming block
 */
void CacheSet::insertFIFO(int tag_value)
{
    insertItem(tag_value);
}

/*
 * Update blocks list according to LRU policy. When hit a block, move
 * the block to the front of the list.
 *
 * Parameters:
 *   index    - the index of the block will be updated in list
 *   itr      - the iterator of the block need to be moved
 */
void CacheSet::updateLRU(int index, list<Block>::iterator itr)
{
    Block *new_block = new Block(itr->tag_value, block_size);
    blocks_of_set.erase(itr);
    blocks_of_set.push_front(*new_block);
}

/*
 * Evict a block in blocks list and update counter.
 *
 * Parameters:
 *   blocks_of_set    - pointer to blocks list
 */
void CacheSet::evict(list<Block> *blocks_of_set)
{
    if ((*blocks_of_set).back().dirty)
    {
        // evict a dirty block, we should write its content back to memory
        // write-through and write-back, evict don't need to incre memory operation
        if (!write_through_flag)
        {
            counter->memoryOperationCycleIncre(block_size);
        }
        // set the dirty back to 0
        (*blocks_of_set).back().dirty = 0;
    }
    (*blocks_of_set).pop_back();
}

/*
 * Try to find a block with a specific tag in current blocks list.
 *
 * Parameters:
 *   tag_value    - tag value of incoming block
 *
 * Returns:
 *   if exits, return the index of the block in blocks list, otherwise return -1.
 */
pair<int, list<Block>::iterator> CacheSet::find(int tag_value)
{
    list<Block>::iterator itr = blocks_of_set.begin();
    int i = 0;
    for (; itr != blocks_of_set.end(); itr++, i++)
    {
        if (itr->tag_value == tag_value)
        {
            return pair<int, list<Block>::iterator>(i, itr);
        }
    }
    // if nothing match, itr is blocks_of_set.end()
    return pair<int, list<Block>::iterator>(-1, itr);
}