/*
 * Standard Header File
 *
 * Structure and code adopted from PBRT by Matt Pharr and Greg Humphreys
 *
 * Brett Warne - bwarne@mit.edu - 10/31/08
 */

#ifndef OM_STD_H
#define OM_STD_H

// Global Includes
#include <cstdio>
#include <cassert>
#include <iostream>
#include <zi/bits/cstdint.hpp>

#include <algorithm>
#include <cmath>

#include <set>
#include <map>
#include <deque>
#include <string>
#include <vector>

// Global Constants
#define BYTES_PER_MB 1048576

namespace om{
static const uint32_t powers_of_2[] = {
    1u,
    2u,
    4u,
    8u,
    16u,
    32u,
    64u,
    128u,
    256u,
    512u,
    1024u,
    2048u,
    4096u,
    8192u,
    16384u,
    32768u,
    65536u,
    131072u,
    262144u,
    524288u,
    1048576u,
    2097152u,
    4194304u,
    8388608u,
    16777216u,
    33554432u,
    67108864u,
    134217728u,
    268435456u,
    536870912u,
    1073741824u,
    2147483648u //2^31
};

// 2^n, domain of [0,31]
inline uint32_t pow2int(const int exp)
{
    assert(0 <= exp && exp < 32);
    return powers_of_2[exp];
}
};

//only valid for a>=0
template <typename T>
inline T ROUNDDOWN(T a, T b)
{
    return a - a % b;
}

//only valid for numToRound >= 0
template <typename T>
inline T ROUNDUP(T numToRound, T multiple)
{
    assert( multiple );
    numToRound += multiple - 1;
    return ROUNDDOWN( numToRound, multiple );
}

namespace om {
namespace math {

// from http://stackoverflow.com/questions/485525/round-for-float-in-c

inline float SymmetricalRound(const float r){
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

inline double SymmetricalRound(const double r){
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

} // namespace math
} // namespace om

namespace om {
namespace vec {

template <typename T>
void eraseRemove(std::vector<T>& v, const T val){
    v.erase(std::remove(v.begin(), v.end(), val), v.end());
}

} // namespace vec
} // namespace om

#endif
