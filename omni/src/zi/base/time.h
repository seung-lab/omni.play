/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef COMMON_BASE_TIME_H_
#define COMMON_BASE_TIME_H_

#include <time.h>
#include <inttypes.h>
#include <sys/types.h>

#include "base.h"

#ifdef WIN32
# include <windows.h>
#else
# include <sys/time.h>
# include <sys/resource.h>
#endif

#ifdef WIN32
static int gettimeofday(struct timeval *tv, void* tz) {

  FILETIME ft;
  LARGE_INTEGER li;
  uint64_t tt;

  GetSystemTimeAsFileTime(&ft);
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  tt = (li.QuadPart - (116444736000000000ULL)) / 10;
  tv->tv_sec = tt / 1000000;
  tv->tv_usec = tt % 1000000;
  return 0;
}
#endif

class ziTimer {
public:

  ziTimer() {
    start();
  }

  inline int64_t lap() {
    int64_t tmp = this->lastLap_;
    this->lastLap_ = ziTimer::NowInUsec();
    return (this->lastLap_ - tmp);
  }

  inline double dLap() {
    return ((double)this->lap()) / 1000000.0;
  }

  inline int64_t total() {
    return (ziTimer::NowInUsec() - this->startTime_);
  }

  inline double dTotal() {
    return ((double)this->total()) / 1000000.0;
  }

  inline void reset() {
    this->startTime_ = this->lastLap_ = ziTimer::NowInUsec();
  }

  inline void start() {
    this->startTime_ = this->lastLap_ = ziTimer::NowInUsec();
  }

  static int64_t NowInSec() {
    return static_cast<int64_t>(time(NULL));
  }

  static int64_t NowInUsec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<int64_t>(tv.tv_sec) * 1000000LL + tv.tv_usec;
  }

#ifndef WIN32
  static int64_t GetCPUSUsage() {
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return static_cast<int64_t>(rusage.ru_stime.tv_sec) * 1000000LL +
      rusage.ru_stime.tv_usec;
  }

  static int64_t GetCPUUUsage() {
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return static_cast<int64_t>(rusage.ru_utime.tv_sec) * 1000000LL +
      rusage.ru_utime.tv_usec;
  }
#endif

private:
  int64_t startTime_, lastLap_;

};

#endif
