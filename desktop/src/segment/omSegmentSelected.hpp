#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "common/common.h"
#include "events/events.h"
#include "segment/omSegmentSelector.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "zi/omUtility.h"

class OmSegmentSelected : private om::singletonBase<OmSegmentSelected> {
 public:
  static void Delete() { instance().sdw_ = SegmentDataWrapper(); }

  static void Set(const SegmentDataWrapper& sdw) { instance().sdw_ = sdw; }

  static SegmentDataWrapper Get() { return instance().sdw_; }

  static SegmentDataWrapper GetSegmentForPainting() {
    return instance().sdwForPainting_;
  }

  static void SetSegmentForPainting(const SegmentDataWrapper& sdw) {
    instance().sdwForPainting_ = sdw;
    om::event::SegmentSelected();
  }

  static void RandomizeColor() {
    if (!instance().sdw_.IsSegmentValid()) {
      return;
    }
    instance().sdw_.RandomizeColor();
    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
  }

  static void AugmentSelection(const SegmentDataWrapper& sdw) {
    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), nullptr,
                          "OmSegmentSelected");
    sel.augmentSelectedSet(sdw.getID(), true);
    sel.sendEvent();
  }

  static void ToggleValid() {
    if (!instance().sdw_.IsSegmentValid()) {
      return;
    }

    OmSegment* seg = instance().sdw_.GetSegment();

    if (seg->IsValidListType()) {
      OmActions::ValidateSegment(instance().sdw_,
                                 om::common::SetValid::SET_NOT_VALID);
    } else {
      OmActions::ValidateSegment(instance().sdw_,
                                 om::common::SetValid::SET_VALID);
    }

    om::event::SegmentModified();
  }

 private:
  OmSegmentSelected() {}
  ~OmSegmentSelected() {}

  SegmentDataWrapper sdw_;
  SegmentDataWrapper sdwForPainting_;

  friend class zi::singleton<OmSegmentSelected>;
};
