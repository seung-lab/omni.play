#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/enums.hpp"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "zi/omUtility.h"
#include "threads/taskManagerTypes.h"

class OmActionsImpl;
class OmSegment;
class OmSegments;
class OmSelectSegmentsParams;
class OmViewGroupState;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmActions : om::singletonBase<OmActions> {
 private:
  static inline OmActionsImpl* impl() {
    return &OmProject::Globals().Actions();
  }

  static om::thread::ThreadPool& pool();

 public:
  // project-related
  static void Save();

  static void Close();
  static void GenerateCloseAction();

  // MST-related
  static void ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                                 const double threshold);

  static void ChangeSizethreshold(const SegmentationDataWrapper sdw,
                                  const double threshold);

  // painting-related
  static void SetVoxels(const om::common::ID segmentationId,
                        const std::set<om::coords::Data>& rVoxels,
                        const om::common::SegID value);

  // segment-related
  static void ValidateSegment(const SegmentDataWrapper& sdw,
                              const om::common::SetValid valid,
                              const bool dontCenter = false);

  static void ValidateSelectedSegments(const SegmentationDataWrapper& sdw,
                                       const om::common::SetValid valid,
                                       const bool dontCenter = false);

  static void UncertainSegment(const SegmentDataWrapper& sdw,
                               const bool uncertain);

  static void UncertainSegmentation(const SegmentationDataWrapper& sdw,
                                    const bool uncertain);

  static void JoinSegments(const SegmentationDataWrapper& sdw);
  static void JoinSegments(const SegmentationDataWrapper& sdw,
                           const om::common::SegIDSet& ids);
  static void JoinSegments(const SegmentationDataWrapper& sdw,
                           OmSegment* seg1, OmSegment* seg2);

  static void FindAndSplitSegments(const SegmentationDataWrapper sdw,
                                   OmSegment* seg1, OmSegment* seg2);
  static void FindAndMultiSplitSegments(const SegmentationDataWrapper sdw,
                                   const om::common::SegIDSet segSet1,
                                   const om::common::SegIDSet segSet2);

  static void ShatterSegment(const SegmentationDataWrapper sdw, OmSegment* seg);

  static void CutSegment(const SegmentDataWrapper& sdw);

  static void SelectSegments(std::shared_ptr<OmSelectSegmentsParams> params);

 private:
  OmActions() {}
  ~OmActions() {}

  friend class zi::singleton<OmActions>;
};
