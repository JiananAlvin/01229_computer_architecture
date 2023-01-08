#include <iostream>

using namespace std;

/*
 * Helper functions for cache simulator.
 */

/*
 * Converts string into integer.
 *
 * Parameters:
 *  s - A string
 *
 * Returns:
 *  -1  - The string cannot be converted to a int.
 *        Otherwise, return the convert result.
 */
int str2Int(string s)
{
    std::size_t pos{};
    try
    {
        int num = std::stoi(s);
    }
    catch (invalid_argument &ex)
    {
        return -1;
    }
    catch (out_of_range &ex)
    {
        return -1;
    }

    int num = stoi(s);
    if (num <= 0)
    {
        cerr << "The argument must be greater than 0" << endl;
        return -1;
    }
    return num;
}

/*
 * Get the result of log_2 (input).
 *
 * Parameters:
 *  s - a 2^n number
 *
 * Returns:
 *  the value of n = log_2 (input)
 */
int intLog2(int input)
{
    int count = 0;
    if (input == 1)
        return 0;
    while (input > 1)
    {
        input = input >> 1; // n/2
        count++;
    }
    return count;
}
