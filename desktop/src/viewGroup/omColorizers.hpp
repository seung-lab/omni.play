#pragma once
#include "precomp.h"

#include "segment/coloring.hpp"

class OmViewGroupState;

class OmColorizers {
 private:
  OmViewGroupState& vgs_;

  zi::spinlock lock_;
  std::array<OmSegmentColorizer*,
             (size_t)om::segment::coloring::NUMBER_OF_ENUMS> colorizers_;

 public:
  OmColorizers(OmViewGroupState& vgs) : vgs_(vgs) {
    std::fill(colorizers_.begin(), colorizers_.end(),
              static_cast<OmSegmentColorizer*>(nullptr));
  }

  ~OmColorizers() {
    FOR_EACH(iter, colorizers_) { delete *iter; }
  }

  inline std::shared_ptr<om::common::ColorARGB> ColorTile(
      uint32_t const* const imageData, const int tileDim,
      const OmTileCoord& key) {
    auto sccType = key.getSegmentColorCacheType();

    {
      zi::guard g(lock_);
      if (!colorizers_[(size_t)sccType]) {
        setupColorizer(tileDim, key, sccType);
      }
    }

    return colorizers_[(size_t)sccType]->ColorTile(imageData);
  }

 private:
  void setupColorizer(const int tileDim, const OmTileCoord& key,
                      const om::segment::coloring sccType) {
    if (om::common::SEGMENTATION != key.getVolume().getVolumeType()) {
      throw om::IoException("can only color segmentations");
    }

    SegmentationDataWrapper sdw(key.getVolume().GetID());
    if (!sdw.IsValidWrapper()) {
      return;
    }

    colorizers_[(size_t)sccType] =
        new OmSegmentColorizer(*sdw.Segments(), sccType, tileDim, vgs_);
  }
};
