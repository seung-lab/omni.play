#pragma once

#include "segment/coloring.hpp"
#include "zi/omMutex.h"

#include <array>

class OmViewGroupState;

class OmColorizers {
 private:
  OmViewGroupState* const vgs_;

  zi::spinlock lock_;
  std::array<OmSegmentColorizer*, om::segment::coloring::NUMBER_OF_ENUMS>
      colorizers_;

 public:
  OmColorizers(OmViewGroupState* vgs) : vgs_(vgs) {
    std::fill(colorizers_.begin(), colorizers_.end(),
              static_cast<OmSegmentColorizer*>(NULL));
  }

  ~OmColorizers() {
    FOR_EACH(iter, colorizers_) { delete *iter; }
  }

  inline OmPooledTile<om::common::ColorARGB>* ColorTile(
      uint32_t const* const imageData, const int tileDim,
      const OmTileCoord& key) {
    const om::segment::coloring sccType = key.getSegmentColorCacheType();

    {
      zi::guard g(lock_);
      if (!colorizers_[sccType]) {
        setupColorizer(tileDim, key, sccType);
      }
    }

    return colorizers_[sccType]->ColorTile(imageData);
  }

 private:
  void setupColorizer(const int tileDim, const OmTileCoord& key,
                      const om::segment::coloring sccType) {
    if (om::common::SEGMENTATION != key.getVolume()->getVolumeType()) {
      throw om::IoException("can only color segmentations");
    }

    SegmentationDataWrapper sdw(key.getVolume()->getID());

    colorizers_[sccType] =
        new OmSegmentColorizer(sdw.Segments(), sccType, tileDim, vgs_);
  }
};
