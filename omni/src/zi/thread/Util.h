/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef CONCURRENCY_UTIL_H_
#define CONCURRENCY_UTIL_H_

#include <string>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <exception>

namespace zi { namespace Threads {

class Util {

  static const int64_t NS_PER_S = 1000000000LL;
  static const int64_t US_PER_S = 1000000LL;
  static const int64_t MS_PER_S = 1000LL;

  static const int64_t NS_PER_MS = NS_PER_S / MS_PER_S;
  static const int64_t US_PER_MS = US_PER_S / MS_PER_S;

public:

  static void toTimespec(struct timespec& result, int64_t value) {
    result.tv_sec = value / MS_PER_S; // ms to s
    result.tv_nsec = (value % MS_PER_S) * NS_PER_MS; // ms to ns
  }

  static void toTimeval(struct timeval& result, int64_t value) {
    result.tv_sec = value / MS_PER_S; // ms to s
    result.tv_usec = (value % MS_PER_S) * US_PER_MS; // ms to us
  }

  static void toMilliseconds(int64_t& result, const struct timespec& value) {
    result = (value.tv_sec * MS_PER_S) + (value.tv_nsec / NS_PER_MS);
    if ((int64_t)(value.tv_nsec) % NS_PER_MS >= (NS_PER_MS / 2)) {
      ++result;
    }
  }

  static void toMilliseconds(int64_t& result, const struct timeval& value) {
    result = (value.tv_sec * MS_PER_S) + (value.tv_usec / US_PER_MS);
    if ((int64_t)(value.tv_usec) % US_PER_MS >= (US_PER_MS / 2)) {
      ++result;
    }
  }

  static int64_t currentTime() {
    int64_t result;

#if defined(HAVE_CLOCK_GETTIME)
    struct timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);
    assert(ret == 0);
    toMilliseconds(result, now);
#else
    struct timeval now;
    int ret = gettimeofday(&now, NULL);
    assert(ret == 0);
    toMilliseconds(result, now);
#endif

    return result;
  }

};

} }

#endif
