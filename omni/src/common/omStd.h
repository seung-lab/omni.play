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
#include <stdarg.h>

#include <stdio.h>

#include <algorithm>
using std::min;
using std::max;
using std::swap;
using std::sort;

#include <assert.h>

#include <iostream>
using std::cout;
using std::endl;
using std::ostream;

#include <map>
using std::map;

#include <math.h>

#include <set>
using std::set;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <vector>
using std::vector;

#define OMPOW(base,expon) (int)pow((double) base, (double)expon)

// Platform-Dependent Definitions
#ifdef __APPLE__
#define powf pow
#define sinf sin
#define cosf cos
#define tanf tan
#define asinf asin
#define acosf acos
#define atanf atan
#define atan2f atan2
#define logf log
#define log10f log10
#define expf exp
#define sqrtf sqrt
#endif // __APPLE__


// Global Type Definitions
// Explicitly-sized versions of integer types
// from 6.828 JOS types.h
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
//typedef long long int64_t;
//typedef unsigned long long uint64_t;

typedef unsigned int u_int;
typedef unsigned long u_long;

// Global Constants
#define MAX_FNAME_SIZE	200
#define BYTES_PER_MB 1048576

#define EPSILON 1.0e-5

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)			\
({					\
	uint32_t __a = (uint32_t) (a);	\
	(typeof(a)) (__a - __a % (n));	\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)				\
({						\
	uint32_t __n = (uint32_t) (n);		\
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})

#endif
