

class GlobalCounter
{
public:
    /// @brief constructor for counter
    GlobalCounter();

    void printConclusion();

    void loadHitIncre();
    void loadMissIncre();
    void storeHitIncre();
    void storeMissIncre();
    void cycleIncre();

    void cacheOperationCycleIncre();
    void memoryOperationCycleIncre(int bit_num_block);

    int load_hit_count;
    int load_miss_count;
    int store_hit_count;
    int store_miss_count;
    int cycle_count;
};