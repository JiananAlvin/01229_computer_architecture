#include <iostream>
#include <unordered_map>
#include "cacheset.h"

using namespace std;
class Cache
{
private:
    unordered_map<int, CacheSet> sets_of_cache;
    GlobalCounter counter;
    // the config property
    int _cache_size;
    int _set_size;
    int _block_size;
    bool _write_allocate_flag;
    bool _write_through_flag;
    bool _lru_flag;
    int _bit_num_index;
    int _bit_num_offset;
    int _bit_num_tag;

    int current_size;

    void doLoadHit();
    void doLoadMiss();

    void doStoreHit(Block *b);
    void doStoreMiss(Block *b);

    void addNewSet(int offset_value, int tag_value, int index_value);

public:
    Cache(int cache_size_input, int set_size_input, int block_size_input, bool write_allocate_flag_input, bool write_through_flag_input, bool lru_flag_input);
    void doLoad(string s);
    void doStore(string s);
    void printConclusion();
    uint32_t getTagValue(string s);
    uint32_t getSetIndexValue(string s);
    uint32_t getBlockOffsetValue(string s);
};
