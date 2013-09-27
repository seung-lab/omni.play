#pragma once

#include "zi/utility.h"

namespace om {
namespace tile {

template <typename T>
class Filters {
 private:
  const int chunkDim_;
  const int elementsPerTile_;

 public:
  Filters(const int chunkDim)
      : chunkDim_(chunkDim), elementsPerTile_(chunkDim * chunkDim) {}

  void Brightness(std::shared_ptr<T> tile, const T absMax,
                  const int32_t shift) {
    zi::transform(tile, tile + elementsPerTile_, tile,
                  ChangeBrightness<T>(absMax, shift));
  }

  void Contrast(std::shared_ptr<T> tile, const T absMax,
                const double contrast) {
    zi::transform(tile, tile + elementsPerTile_, tile,
                  ChangeContrast<T>(absMax, contrast));
  }

  void Gamma(std::shared_ptr<T> tile, const double gamma) {
    zi::transform(tile, tile + elementsPerTile_, tile, ChangeGamma<T>(gamma));
  }

  template <class C>
  void recast(const T* oldTile, C* newTile) const {
    std::copy(oldTile, oldTile + elementsPerTile_, newTile);
  }

  std::shared_ptr<T> recast(std::shared_ptr<T> oldTile) const {
    return oldTile;
  }

  template <typename C>
  void rescaleAndCast(const T* oldTile, const T rangeMin, const T rangeMax,
                      const T absMax, C* newTile) const {
    zi::transform(oldTile, oldTile + elementsPerTile_, newTile,
                  Rescale<T, C>(rangeMin, rangeMax, absMax));
  }

 private:
  template <typename U, typename C>
  class Rescale {
   private:
    const U rangeMin_, rangeMax_, absMax_;

   public:
    Rescale(const U rangeMin, const U rangeMax, const U absMax)
        : rangeMin_(rangeMin), rangeMax_(rangeMax), absMax_(absMax) {}

    C operator()(U val) const {
      return (val - rangeMin_) * ((double)absMax_ / (rangeMax_ - rangeMin_));
    }
  };

  // brightness, contrast, and gamma based on
  // http://pippin.gimp.org/image_processing/chap_point.html#id2556807

  // TODO: clamp should not be needed

  template <typename U>
  inline static T clamp(T absMax, U val) {
    if (val > absMax) {
      return absMax;
    }

    if (val < 0) {
      return 0;
    }

    return val;
  }

  template <typename U>
  class ChangeBrightness {
   public:
    ChangeBrightness(const U absMax, const int32_t shift)
        : absMax_(absMax), shift_(shift) {}

    U operator()(U val) const {
      if (!val) {
        return 0;
      }

      return clamp(absMax_, val + shift_);
    }

   private:
    const U absMax_;
    int32_t shift_;
  };

  template <typename U>
  class ChangeContrast {
   public:
    ChangeContrast(const double absMax, const double contrast)
        : absMax_(absMax), halfAbsMax_(absMax_ / 2.0), contrast_(contrast) {}

    U operator()(U val) const {
      return clamp(255, (static_cast<double>(val) - halfAbsMax_) * contrast_ +
                            halfAbsMax_);
    }

   private:
    const double absMax_;
    const double halfAbsMax_;
    const double contrast_;
  };

  template <typename U>
  class ChangeGamma {
   public:
    ChangeGamma(const double gamma) : gamma_(gamma) {}

    U operator()(U val) const {
      return clamp(255,
                   255.0 * std::pow(static_cast<double>(val) / 255.0, gamma_));
    }

   private:
    const double gamma_;
  };
};
}
}
