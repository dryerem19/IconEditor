#ifndef UTILS_H
#define UTILS_H

namespace utils
{
    template<typename T>
    T mapToRange(T value, T inMin, T inMax, T outMin, T outMax) {
        if (qFuzzyCompare(inMin, inMax)) {
            return outMin;
        }

        return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
    }

    bool isPowerOfTwo(int value);
}
#endif // UTILS_H
