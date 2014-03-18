#pragma once
#include "precomp.h"

/*
 *  OmTimer provides a simple cross-platform high-resolution timer for use in
 *    performance testing by wrapping OS-specific timer functions.
 */

#include "common/common.h"

class OmTimer {
 private:
  zi::wall_timer timer_;

 public:
  OmTimer() : timer_() {}

  ~OmTimer() {}

  void start() { timer_.reset(); }

  void restart() { timer_.restart(); }

  void reset() { timer_.restart(); }

  double us_elapsed() { return timer_.elapsed<zi::in_usecs>(); }

  double ms_elapsed() { return timer_.elapsed<zi::in_msecs>(); }

  double s_elapsed() { return timer_.elapsed<zi::in_usecs>() / 1000000.; }

  void Print(const std::string str) {
    const double timeSecs = s_elapsed();

    log_info("%s in %.2f secs", str, timeSecs);
  }

  void PrintV(const std::string str) {
    const double timeSecs = s_elapsed();

    log_info("%s in %.10f secs", str, timeSecs);
  }

  void PrintDone() { Print("done"); }
};
