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
#include <string>
#include <vector>

// Global Constants
#define BYTES_PER_MB 1048576

namespace om{
	static const uint32_t powers_of_2[] = {
		1,
		2,
		4,
		8,
		16,
		32,
		64,
		128,
		256,
		512,
		1024,
		2048,
		4096,
		8192,
		16384,
		32768,
		65536,
		131072,
		262144,
		524288,
		1048576,
		2097152,
		4194304,
		8388608,
		16777216,
		33554432,
		67108864,
		134217728,
		268435456,
		536870912,
		1073741824,
		2147483648 //2^31
	};

	// 2^n, domain of [0,31]
	inline uint32_t pow2(const int exp)
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

#endif
