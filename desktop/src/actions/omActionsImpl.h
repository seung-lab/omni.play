#pragma once

#include "common/om.hpp"
#include "common/common.h"

class OmSegment;
class OmSelectSegmentsParams;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmViewGroupState;
class OmSegments;

class OmActionsImpl {
private:
    boost::scoped_ptr<OmThreadPool> threadPool_;

public:
    OmActionsImpl();
    ~OmActionsImpl();

    inline OmThreadPool& ThreadPool(){
        return *threadPool_;
    }

// project-related
    void Save();
    void Close();

// MST-related
    void ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                            const double threshold);

    void ChangeSizethreshold(const SegmentationDataWrapper sdw,
                             const double threshold);

// painting-related
    void SetVoxel(const OmID segmentationId,
                  const om::coords::Global rVoxel,
                  const OmSegID value);

    void SetVoxels(const OmID segmentationId,
                   const std::set<om::coords::Global> rVoxels,
                   const OmSegID value);

// segment-related
    void ValidateSegment(const SegmentDataWrapper sdw,
                         const om::common::SetValid valid, const bool dontCenter=false);

    void ValidateSelectedSegments(const SegmentationDataWrapper sdw,
                                  const om::common::SetValid valid);

    void UncertainSegment(const SegmentDataWrapper sdw,
                          const bool uncertain);

    void UncertainSegmentation(const SegmentationDataWrapper sdw,
                               const bool uncertain);

    void JoinSegmentsWrapper(const SegmentationDataWrapper sdw);
    void JoinSegmentsSet(const SegmentationDataWrapper sdw,
                         const OmSegIDsSet ids);

    void FindAndSplitSegments(OmSegment* seg1, OmSegment* seg2);
    void ShatterSegment(OmSegment* seg);

    void CutSegment(const SegmentDataWrapper sdw);

    void SelectSegments(om::shared_ptr<OmSelectSegmentsParams> params);

// group-related
    void CreateOrDeleteSegmentGroup(const OmID segmentationID,
                                    const OmSegIDsSet selectedSegmentIDs,
                                    const OmGroupName name,
                                    const bool create);

private:
    void setUncertain(const SegmentDataWrapper& sdw,
                      const bool uncertain);

    void setUncertain(const SegmentationDataWrapper& sdw,
                      const bool uncertain);

};

