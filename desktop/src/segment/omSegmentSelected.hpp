#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "events/events.h"
#include "segment/omSegmentSelector.h"
#include "system/cache/omCacheManager.h"
#include "zi/utility.h"
#include "utility/segmentDataWrapper.hpp"

class OmSegmentSelected : private om::SingletonBase<OmSegmentSelected> {
 public:
  static void Delete() { instance().sdw_ = SegmentDataWrapper(); }

  static void Set(const SegmentDataWrapper& sdw) { instance().sdw_ = sdw; }

  static SegmentDataWrapper Get() { return instance().sdw_; }

  static SegmentDataWrapper GetSegmentForPainting() {
    return instance().sdwForPainting_;
  }

  /**
   * @brief SetSegmentForPainting Emit the signal for openning the brush dialog
   * @param sdw
   */
  static void SetSegmentForPainting(const SegmentDataWrapper& sdw) {
    instance().sdwForPainting_ = sdw;
    om::event::SegmentBrush();
  }

  static void RandomizeColor() {
    if (!instance().sdw_.IsSegmentValid()) {
      return;
    }
    instance().sdw_.RandomizeColor();
    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
  }

  static void AugmentSelection(const SegmentDataWrapper& sdw);
  static void ToggleValid();

 private:
  OmSegmentSelected() {}
  ~OmSegmentSelected() {}

  SegmentDataWrapper sdw_;
  SegmentDataWrapper sdwForPainting_;

  friend class zi::singleton<OmSegmentSelected>;
};
