#pragma once
#include "precomp.h"

#include "common/common.h"
#include "utility/dataWrappers.h"

struct OmSelectSegmentsParams {
  SegmentDataWrapper sdw;
  om::common::SegIDMap newSelectedIDs;
  om::common::SegIDMap oldSelectedIDs;
  om::common::SegIDSet wtfSet;
  std::map<int, int> mapthing;
  void* sender;
  std::string comment;
  bool shouldScroll;
  bool addToRecentList;
  bool autoCenter;

  om::common::AddOrSubtract addOrSubtract;
};

struct OmSegmentGUIparams {
  SegmentationDataWrapper sdw;
  bool stayOnPage;
};
