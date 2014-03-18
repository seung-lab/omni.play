#pragma once
#include "precomp.h"

#include "common/common.h"

extern template class std::vector<uint8_t>;

class OmSegmentColors {
 private:
  static inline uint8_t makeSelectedColor(const uint8_t val) {
    static const double selectedSegColorFactor = 2.5;

    return val > 101 ? 255 : static_cast<uint8_t>(val * selectedSegColorFactor);
  }

 public:
  static inline uint8_t MakeMutedColor(const uint8_t val) { return val / 2; }

  static std::vector<uint8_t> makeLookupTable() {
    std::vector<uint8_t> ret(256, 0);
    for (auto i = 0; i < 256; ++i) {
      ret[i] = makeSelectedColor(i);
    }
    return ret;
  }
};
