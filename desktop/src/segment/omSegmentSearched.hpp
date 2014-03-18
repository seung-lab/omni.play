#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "common/common.h"
#include "events/events.h"
#include "zi/utility.h"
#include "utility/segmentDataWrapper.hpp"

class OmSegmentSearched : private om::SingletonBase<OmSegmentSearched> {
 public:
  static void Delete() { instance().sdw_ = SegmentDataWrapper(); }

  static void Set(const SegmentDataWrapper& sdw) { instance().sdw_ = sdw; }

  static SegmentDataWrapper Get() { return instance().sdw_; }

 private:
  OmSegmentSearched() {}
  ~OmSegmentSearched() {}

  SegmentDataWrapper sdw_;

  friend class zi::singleton<OmSegmentSearched>;
};
