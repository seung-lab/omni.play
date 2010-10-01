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

#include <algorithm>
#include <cmath>

#include <set>
#include <map>
#include <string>
#include <vector>

// Global Constants
#define BYTES_PER_MB 1048576

template <typename T>
inline T ROUNDDOWN(T a, T b)
{
	return a - a % b;
}

// http://stackoverflow.com/questions/3407012/c-rounding-up-to-the-nearest-multiple-of-a-number
template <typename T>
inline T ROUNDUP(T numToRound, T multiple)
{
	assert( multiple );
	numToRound += multiple - 1;
	return ROUNDDOWN( numToRound, multiple );
}

#endif
