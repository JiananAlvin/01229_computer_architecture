#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

class Block
{

public:
    int tag_value;
    bool dirty;
    int block_size;

    Block(int tag_value_input, int block_size_input);
};

#endif