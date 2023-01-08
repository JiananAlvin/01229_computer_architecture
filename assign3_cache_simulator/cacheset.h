#ifndef SET_H
#define SET_H

#include <unordered_map>
#include "block.h"
#include <list>
#include "globalcounter.h"

using namespace std;
class CacheSet
{
public:
    list<Block> blocks_of_set;
    int set_size;
    int block_size;
    int oldest_block_tag;
    int current_size;
    GlobalCounter *counter;
    bool write_through_flag;

    CacheSet(int set_size_input, int block_size_input, GlobalCounter *counter, bool write_through_flag);
    void insertLRU(int tag_value);
    void updateLRU(int index, list<Block>::iterator itr);
    void insertFIFO(int tag_value);
    void insertItem(int tag_value);
    void evict(list<Block> *blocks_of_set);
    pair<int, list<Block>::iterator> find(int tag_value);
};

#endif