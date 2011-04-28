#ifndef OM_ACTIONS_IMPL_H
#define OM_ACTIONS_IMPL_H

#include "common/om.hpp"
#include "common/omCommon.h"

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

// painting-related
    void SetVoxel(const OmID segmentationId,
                         const DataCoord rVoxel,
                         const OmSegID value);

    void SetVoxels(const OmID segmentationId,
                          const std::set<DataCoord> rVoxels,
                          const OmSegID value);

// segment-related
    void ValidateSegment(const SegmentDataWrapper sdw,
                                const om::SetValid valid, const bool dontCenter=false);

    void ValidateSelectedSegments(const SegmentationDataWrapper sdw,
                                         const om::SetValid valid);

    void UncertainSegment(const SegmentDataWrapper sdw,
                                 const bool uncertain);

    void UncertainSegmentation(const SegmentationDataWrapper sdw,
                                 const bool uncertain);

    void JoinSegmentsSet(const OmID segmentationID,
                             const OmSegIDsSet ids);

    void JoinSegmentsID(const OmID segmentationID);
    void JoinSegmentsWrapper(const SegmentationDataWrapper sdw);

    void FindAndSplitSegments(const SegmentDataWrapper sdw,
                                     OmViewGroupState* vgs, const DataCoord coord);
    void FindAndCutSegments(const SegmentDataWrapper sdw,
                                   OmViewGroupState* vgs);

    void SelectSegments(om::shared_ptr<OmSelectSegmentsParams> params);

// group-related
    void CreateOrDeleteSegmentGroup(const OmID segmentationID,
                                           const OmSegIDsSet selectedSegmentIDs,
                                           const OmGroupName name,
                                           const bool create);
private:
    OmSegPtrSet setNotValid(OmSegments * cache,
                                   const OmSegIDsSet& ids);

    void setAsValid(const OmSegPtrSet& ids);

    void runIfSplittable(OmSegment* seg1, OmSegment* seg2,
                                const DataCoord& coord1, const DataCoord& coord2);

    void doFindAndCutSegment(const SegmentDataWrapper& sdw,
                                    OmViewGroupState* vgs);

    void setUncertain(const SegmentDataWrapper& sdw,
                             const bool uncertain);

    void setUncertain(const SegmentationDataWrapper& sdw,
                             const bool uncertain);

    void doJoinSegments(const SegmentationDataWrapper& sdw,
                               const OmSegIDsSet& ids);

};

#endif
