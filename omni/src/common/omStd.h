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

#include <map>
using std::map;

#include <math.h>

#include <set>
using std::set;

#include <string>
using std::string;

#include <vector>
using std::vector;

#define OMPOW(base,expon) (int)pow((double) base, (double)expon)

// Platform-Dependent Definitions
#ifdef __APPLE__
# define powf pow
# define sinf sin
# define cosf cos
# define tanf tan
# define asinf asin
# define acosf acos
# define atanf atan
# define atan2f atan2
# define logf log
# define log10f log10
# define expf exp
# define sqrtf sqrt
#else
# define HAVE_CLOCK_GETTIME
#endif // __APPLE__

#ifdef WIN32
#include <float.h>
#define ISNAN(x) _isnan(x)
#define snprintf _snprintf
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286
#endif
static inline double round(double val)
{   
    return floor(val + 0.5);
}
#else
#define ISNAN(x) std::isnan(x)
#endif

// Global Type Definitions
// Explicitly-sized versions of integer types
// from QT

typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */
typedef qint64 qlonglong;
typedef quint64 qulonglong;

// Global Constants
#define BYTES_PER_MB 1048576

#define EPSILON 1.0e-5

#ifdef WIN32
inline quint32 ROUNDDOWN (quint32 a, quint32 n)
{
	quint32 __a = (quint32) (a);
	return (__a - __a % (n));
}
inline quint32 ROUNDUP (quint32 a, quint32 n)
{
	quint32 __n = (quint32) (n);
	return (ROUNDDOWN((quint32) (a) + __n - 1, __n));
}

#else
// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)			\
({					\
	quint32 __a = (quint32) (a);	\
	(typeof(a)) (__a - __a % (n));	\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)				\
({						\
	quint32 __n = (quint32) (n);		\
	(typeof(a)) (ROUNDDOWN((quint32) (a) + __n - 1, __n));	\
})
#endif

#endif
