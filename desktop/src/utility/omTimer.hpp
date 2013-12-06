#pragma once

/*
 *  OmTimer provides a simple cross-platform high-resolution timer for use in
 *    performance testing by wrapping OS-specific timer functions.
 */

#include "common/common.h"

#include <zi/time/time.hpp>

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

    log_infos << str << " in " << std::fixed << std::setprecision(2) << timeSecs
              << " secs\n";
  }

  void PrintV(const std::string str) {
    const double timeSecs = s_elapsed();

    log_infos << str << " in " << std::fixed << std::setprecision(10)
              << timeSecs << " secs\n";
  }

  void PrintDone() { Print("done"); }
};
