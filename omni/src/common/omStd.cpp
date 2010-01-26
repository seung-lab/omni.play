
#include "omStd.h"

// Error Reporting Includes
#include <stdarg.h>
#include "system/omDebug.h"

// Error Reporting Definitions
#define ERROR_IGNORE 0
#define ERROR_CONTINUE 1
#define ERROR_ABORT 2

// Error Reporting Functions
static void processError(const char *format, va_list args,
		const char *message, int disposition) {

	char errorBuf[2048];
	vsnprintf(errorBuf, sizeof(errorBuf), format, args);

	// Report error
	switch (disposition) {
	case ERROR_IGNORE:
		return;
	case ERROR_CONTINUE:
		fprintf(stderr, "%s: %s\n", message, errorBuf);
		break;
	case ERROR_ABORT:
		fprintf(stderr, "%s: %s\n", message, errorBuf);
		exit(1);
	}
}

void Info(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Notice", ERROR_CONTINUE);
	va_end(args);
}
void Warning(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Warning", ERROR_CONTINUE);
	va_end(args);
}
void Error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Error", ERROR_CONTINUE);
	va_end(args);
}
void Severe(const char *format, ...) {
	va_list args;
	va_start(args, format);
	processError(format, args, "Fatal Error", ERROR_ABORT);
	va_end(args);
}





// Random Number State
/*
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */
// Random Number Functions
static void init_genrand(u_long seed) {
	mt[0]= seed & 0xffffffffUL;
	for (mti=1; mti<N; mti++) {
		mt[mti] =
		(1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		mt[mti] &= 0xffffffffUL;
		/* for >32 bit machines */
	}
}
unsigned long genrand_int32(void)
{
	unsigned long y;
	static unsigned long mag01[2]={0x0UL, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if (mti >= N) { /* generate N words at one time */
		int kk;

		if (mti == N+1)   /* if init_genrand() has not been called, */
			init_genrand(5489UL); /* default initial seed */

		for (kk=0;kk<N-M;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (;kk<N-1;kk++) {
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

		mti = 0;
	}

	y = mt[mti++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return y;
}
/* generates a random number on [0,1]-real-interval */
float genrand_real1(void)
{
	return genrand_int32()*((float)1.0/(float)4294967295.0);
	/* divided by 2^32-1 */
}
/* generates a random number on [0,1)-real-interval */
float genrand_real2(void)
{
	return genrand_int32()*((float)1.0/(float)4294967296.0);
	/* divided by 2^32 */
}

