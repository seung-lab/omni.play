#pragma once

#include "zi/utility.h"

class meshParams : private om::singletonBase<meshParams> {
 public:
  static void SetDownScallingFactor(const double factor) {
    instance().factor_ = factor;
  }
  static double GetDownScallingFactor() { return instance().factor_; }

 private:
  meshParams() : factor_(2.5) {}

  double factor_;

  friend class zi::singleton<meshParams>;
};
