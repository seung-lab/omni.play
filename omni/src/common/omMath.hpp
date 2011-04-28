#ifndef OM_MATH_HPP
#define OM_MATH_HPP

namespace om {
namespace math {

static const uint64_t bytesPerMB = 1048576ull;

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

// only valid for numToRound >= 0
template <typename T>
inline T roundDown(T numToRound, T multiple)
{
    return numToRound - numToRound % multiple;
}

// only valid for numToRound >= 0
template <typename T>
inline T roundUp(T numToRound, T multiple)
{
    assert( multiple );
    numToRound += multiple - 1;
    return roundDown( numToRound, multiple );
}

// from http://stackoverflow.com/questions/485525/round-for-float-in-c
inline float symmetricalRound(const float r){
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

inline double symmetricalRound(const double r){
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

} // namespace math
} // namespace om

#endif
