/*
 * cache simulator
 * CSF Assignment 3 MS3
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <iostream>
#include <vector>
#include "cache.h"

using namespace std;

bool checkInputArgs(char *argv[]);
bool checkTwoExponation(int num);

int main(int argc, char *argv[])
{
    // number of sets in the cache (a positive power-of-2)
    // number of blocks in each set (a positive power-of-2)
    // number of bytes in each block (a positive power-of-2, at least 4)
    // write-allocate or no-write-allocate
    // write-through or write-back
    // lru (least-recently-used) or fifo evictions
    if (!checkInputArgs(argv))
        return -1;

    int cache_size, set_size, block_size;
    bool write_allocate_flag, write_through_flag, lru_flag;
    cache_size = stoi(argv[1]);
    set_size = stoi(argv[2]);
    block_size = stoi(argv[3]);

    string write_allocate = argv[4];
    string write_through = argv[5];
    string lru = argv[6];
    write_allocate_flag = write_allocate.compare("write-allocate") == 0;
    write_through_flag = write_through.compare("write-through") == 0;
    lru_flag = lru.compare("lru") == 0;

    Cache cache = Cache(cache_size, set_size, block_size, write_allocate_flag, write_through_flag, lru_flag);

    for (string line; getline(cin, line);)
    {
        string space_delimiter = " ";
        vector<string> words{};

        size_t pos = 0;
        while ((pos = line.find(space_delimiter)) != string::npos)
        {
            words.push_back(line.substr(0, pos));
            line.erase(0, pos + space_delimiter.length());
        }
        if (words[0].compare("l") == 0)
        {
            cache.doLoad(words[1]);
        }
        else if (words[0].compare("s") == 0)
        {
            cache.doStore(words[1]);
        }
    }

    // print conclusion
    cache.printConclusion();
}

/*
 * Check Input args.
 *
 * Parameters:
 *   s -  input command
 *
 * Returns:
 *   if input args are valid, return true, otherwise, return false.
 */
bool checkInputArgs(char *argv[])
{
    int cache_size, set_size, block_size;
    bool write_allocate_flag, write_through_flag, lru_flag;
    try
    { // check if input is number
        cache_size = stoi(argv[1]);
        set_size = stoi(argv[2]);
        block_size = stoi(argv[3]);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "defined illegal size!" << '\n';
        return false;
    }
    // check if the number is 2^n
    if (!checkTwoExponation(cache_size))
    {
        std::cerr << "cache size is not 2^n!" << '\n';
        return false;
    }
    if (!checkTwoExponation(set_size))
    {
        std::cerr << "set size is not 2^n!" << '\n';
        return false;
    }
    if (cache_size < 1 || set_size < 1 || block_size < 1)
    {
        std::cerr << "size can not be smaller than 1" << '\n';
        return false;
    }
    if (block_size < 4)
    {
        std::cerr << "block_size can not be smaller than 4" << '\n';
        return false;
    }
    string write_allocate = argv[4];
    string write_through = argv[5];
    string lru = argv[6];
    // check
    if (write_allocate.compare("write-allocate") != 0 && write_allocate.compare("no-write-allocate") != 0)
    {
        std::cerr << "unknown arg 4!" << '\n';
        return false;
    }
    if (write_through.compare("write-through") != 0 && write_through.compare("write-back") != 0)
    {
        std::cerr << "unknown arg 5!" << '\n';
        return false;
    }
    if (write_allocate.compare("no-write-allocate") == 0 && write_through.compare("write-back") == 0)
    {
        std::cerr << "It doesn’t make sense to combine no-write-allocate with write-back!" << '\n';
        return false;
    }
    if (lru.compare("lru") != 0 && lru.compare("fifo") != 0)
    {
        std::cerr << "unknown arg 6!" << '\n';
        return false;
    }
    return true;
}

/*
 * Check if a number is 2^n:
 *   We could find such rule: if a number A is 2^n, it must be like 100000...0000.
 *   And A - 1 = 01111....1111. So A & (A-1) = 0.
 *
 * Parameters:
 *   s -  input number
 *
 * Returns:
 *   if input the number is a certain power of two, return true, otherwise, return false.
 */
bool checkTwoExponation(int num)
{
    return !(num & (num - 1));
}
