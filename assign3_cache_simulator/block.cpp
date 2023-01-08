#include "block.h"

// Constructor of Block
Block::Block(int tag_value_input, int block_size_input)
    : block_size(block_size_input), dirty(0), tag_value(tag_value_input)
{
}
