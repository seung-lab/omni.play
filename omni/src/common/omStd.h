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

#ifdef M_PI
#undef M_PI
#endif
#define M_PI			3.14159265358979323846f
#define INV_PI			0.31830988618379067154f
#define INV_TWOPI		0.15915494309189533577f
#ifndef INFINITY
#define INFINITY		FLT_MAX
#endif
#define EPSILON 1.0e-5
#define VERSION 1.0

// Global Forward Declarations

// Global Function Declarations
#ifdef __GNUG__
#define PRINTF_FUNC __attribute__ \
	((__format__ (__printf__, 1, 2)))
#else
#define PRINTF_FUNC
#endif // __GNUG__

#define DEPRECATED(func) func __attribute__ ((deprecated))

/*
void Info(const char *, ...) PRINTF_FUNC;
void Warning(const char *, ...) PRINTF_FUNC;
void Error(const char *, ...) PRINTF_FUNC;
void Severe(const char *, ...) PRINTF_FUNC;

unsigned long genrand_int32(void);
extern float genrand_real1(void);
extern float genrand_real2(void);
*/

// Global Classes




// Global Macro Functions
#ifdef NDEBUG
#define Assert(expr) ((void)0)
#else
#define Assert(expr) \
    ((expr) ? (void)0 : \
		Severe("Assertion \"%s\" failed in:\n%s : line %d", \
               #expr, __FILE__, __LINE__))
#endif // NDEBUG


#define DOUT(_msg)	if(DEBUG) cout << _msg << endl;


// from MIT 6.828 JOS types.h
// Efficient min and max operations
#define MIN(_a, _b)							\
({											\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})

#define MAX(_a, _b)							\
({											\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)							\
({												\
	uint32_t __a = (uint32_t) (a);				\
	(typeof(a)) (__a - __a % (n));				\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)							\
({												\
	uint32_t __n = (uint32_t) (n);				\
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})



// Global Inline Functions

inline float Lerp(float t, float v1, float v2) {
	return (1.f - t) * v1 + t * v2;
}
inline float Clamp(float val, float low, float high) {
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}
inline int Clamp(int val, int low, int high) {
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}
inline int Mod(int a, int b) {
    int n = int(a/b);
    a -= n*b;
    if (a < 0)
        a += b;
    return a;
}
inline float Radians(float deg) {
	return ((float)M_PI/180.f) * deg;
}
inline float Degrees(float rad) {
	return (180.f/(float)M_PI) * rad;
}
inline float Log2(float x) {
	static float invLog2 = 1.f / logf(2.f);
	return logf(x) * invLog2;
}
inline int Log2Int(float v) {
#if 0
 	return ((*reinterpret_cast<int *>(&v)) >> 23) - 127;
#else
#define _doublemagicroundeps	      (.5-1.4e-11)
	return int(Log2(v) + _doublemagicroundeps);
#endif
}
inline bool IsPowerOf2(int v) {
	return (v & (v - 1)) == 0;
}
inline uint32_t RoundUpPow2(uint32_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v+1;
}
#if (defined(__linux__) && defined(__i386__)) || defined(WIN32)
//#define FAST_INT 1
#endif
#define _doublemagicroundeps	      (.5-1.4e-11)
	//almost .5f = .5f - 1e^(number of exp bit)
inline int Round2Int(double val) {
#ifdef FAST_INT
#define _doublemagic			double (6755399441055744.0)
	//2^52 * 1.5,  uses limited precision to floor
	val		= val + _doublemagic;
	return (reinterpret_cast<long*>(&val))[0];
#else
	return int (val+_doublemagicroundeps);
#endif
}
inline int Float2Int(double val) {
#ifdef FAST_INT
	return (val<0) ?  Round2Int(val+_doublemagicroundeps) :
		   Round2Int(val-_doublemagicroundeps);
#else
	return (int)val;
#endif
}
inline int Floor2Int(double val) {
#ifdef FAST_INT
	return Round2Int(val - _doublemagicroundeps);
#else
	return (int)floor(val);
#endif
}
inline int Ceil2Int(double val) {
#ifdef FAST_INT
	return Round2Int(val + _doublemagicroundeps);
#else
	return (int)ceil(val);
#endif
}
/*
inline float RandomFloat();
inline unsigned long RandomUInt();
inline float RandomFloat() {
	return genrand_real2();
}

inline unsigned long RandomUInt() {
	return genrand_int32();
}
 */

inline bool Quadratic(float A, float B, float C, float *t0,
		float *t1) {
	// Find quadratic discriminant
	float discrim = B * B - 4.f * A * C;
	if (discrim < 0.) return false;
	float rootDiscrim = sqrtf(discrim);
	// Compute quadratic _t_ values
	float q;
	if (B < 0) q = -.5f * (B - rootDiscrim);
	else       q = -.5f * (B + rootDiscrim);
	*t0 = q / A;
	*t1 = C / q;
	if (*t0 > *t1) swap(*t0, *t1);
	return true;
}
inline float SmoothStep(float min, float max, float value) {
	float v = Clamp((value - min) / (max - min), 0.f, 1.f);
	return v * v * (-2.f * v  + 3.f);
}
inline float ExponentialAverage(float avg,
                              float val, float alpha) {
	return (1.f - alpha) * val + alpha * avg;
}



/*bwarne*/
inline string i2str(uint32_t i) {
	ostringstream ss;	
	ss << i;
	return ss.str();
}

/*
 *	Use vsprintf to format a variable argument string.
 */
inline void printf2str(string &str, const char *fmt, ...)		
{
	char text[256];										// Holds Our String
    va_list ap;											// Pointer To List Of Arguments
	
	va_start(ap, fmt);                                  // Parses The String For Variables
	vsprintf(text, fmt, ap);                            // And Converts Symbols To Actual Numbers
    va_end(ap);                                         // Results Are Stored In Text
	
	//store formatted text
	str = text;
}	


inline string printf2str(const int strmaxlen, const char *fmt = NULL, ...)		
{
	char text[strmaxlen];									// Holds Our String
    va_list ap;											// Pointer To List Of Arguments
	
	va_start(ap, fmt);                                  // Parses The String For Variables
	vsprintf(text, fmt, ap);                            // And Converts Symbols To Actual Numbers
    va_end(ap);                                         // Results Are Stored In Text
	
	//store formatted text
	return string(text);
}


#endif
