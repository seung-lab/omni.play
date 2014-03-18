#pragma once
#include "precomp.h"

#include "tiles/omTileFilters.hpp"
#include "zi/omUtility.h"

class OmChannelTileFilter : private om::singletonBase<OmChannelTileFilter> {
 private:
  static const uint8_t absMax_ = 255;
  static const int32_t defaultBrightness_ = 0;
  constexpr static const double defaultContrast_ = 1;
  constexpr static const double defaultGamma_ = 1.0;

 public:
  static void Filter(std::shared_ptr<uint8_t> slice) {
    // TODO: don't recreate filter every time
    OmTileFilters<uint8_t> filter(128);

    if (defaultBrightness_ != instance().brightnessShift_) {
      filter.Brightness(slice.get(), absMax_, instance().brightnessShift_);
    }

    if (!qFuzzyCompare(defaultContrast_, instance().contrastValue_)) {
      filter.Contrast(slice.get(), absMax_, instance().contrastValue_);
    }

    if (!qFuzzyCompare(defaultGamma_, instance().gamma_)) {
      filter.Gamma(slice.get(), instance().gamma_);
    }
  }

  static int32_t GetBrightnessShift() { return instance().brightnessShift_; }
  static void SetBrightnessShift(const int32_t shift) {
    instance().brightnessShift_ = shift;
  }

  static double GetContrastValue() { return instance().contrastValue_; }
  static void SetContrastValue(const double contrast) {
    instance().contrastValue_ = contrast;
  }

  static double GetGamma() { return instance().gamma_; }
  static void SetGamma(const double gamma) { instance().gamma_ = gamma; }

 private:
  int32_t brightnessShift_;
  double contrastValue_;
  double gamma_;

  OmChannelTileFilter()
      : brightnessShift_(0), contrastValue_(1.0), gamma_(1.0) {}

  friend class zi::singleton<OmChannelTileFilter>;
};
