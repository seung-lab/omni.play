#pragma once
#include "precomp.h"

#include "common/common.h"

class OmSegment;
class OmSelectSegmentsParams;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmViewGroupState;
class OmSegments;

class OmActionsImpl {
 private:
  std::unique_ptr<OmThreadPool> threadPool_;

 public:
  OmActionsImpl();
  ~OmActionsImpl();

  inline OmThreadPool& ThreadPool() { return *threadPool_; }

  // project-related
  void Save();
  void Close();

  // MST-related
  void ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                          const double threshold);

  void ChangeSizethreshold(const SegmentationDataWrapper sdw,
                           const double threshold);

  // painting-related
  void SetVoxel(const om::common::ID segmentationId,
                const om::coords::Global rVoxel, const om::common::SegID value);

  void SetVoxels(const om::common::ID segmentationId,
                 const std::set<om::coords::Global> rVoxels,
                 const om::common::SegID value);

  // segment-related
  void ValidateSegment(const SegmentDataWrapper sdw,
                       const om::common::SetValid valid,
                       const bool dontCenter = false);

  void ValidateSelectedSegments(const SegmentationDataWrapper sdw,
                                const om::common::SetValid valid);

  void UncertainSegment(const SegmentDataWrapper sdw, const bool uncertain);

  void UncertainSegmentation(const SegmentationDataWrapper sdw,
                             const bool uncertain);

  void JoinSegmentsWrapper(const SegmentationDataWrapper sdw);
  void JoinSegmentsSet(const SegmentationDataWrapper sdw,
                       const om::common::SegIDSet ids);

  void FindAndSplitSegments(const SegmentationDataWrapper sdw, OmSegment* seg1,
                            OmSegment* seg2);
  void ShatterSegment(const SegmentationDataWrapper sdw, OmSegment* seg);

  void CutSegment(const SegmentDataWrapper sdw);

  void SelectSegments(std::shared_ptr<OmSelectSegmentsParams> params);

 private:
  void setUncertain(const SegmentDataWrapper& sdw, const bool uncertain);

  void setUncertain(const SegmentationDataWrapper& sdw, const bool uncertain);
};
