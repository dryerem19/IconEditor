#include "utils.h"

bool utils::isPowerOfTwo(int value)
{
    return value > 0 && (value & (value - 1)) == 0;
}
