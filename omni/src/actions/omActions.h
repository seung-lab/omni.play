#ifndef OM_ACTIONS_HPP
#define OM_ACTIONS_HPP

#include "common/om.hpp"
#include "common/omCommon.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "zi/omUtility.h"

class OmActionsImpl;
class OmSegment;
class OmSegments;
class OmSelectSegmentsParams;
class OmThreadPool;
class OmViewGroupState;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmActions : om::singletonBase<OmActions>{
private:
    static inline OmActionsImpl* impl(){
        return &OmProject::Globals().Actions();
    }

    static OmThreadPool& pool();

public:
// project-related
    static void Save();

    static void Close();
    static void GenerateCloseAction();

// MST-related
    static void ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                                   const double threshold);

// painting-related
    static void SetVoxel(const OmID segmentationId,
                         const DataCoord& rVoxel,
                         const OmSegID value);

    static void SetVoxels(const OmID segmentationId,
                          const std::set<DataCoord>& rVoxels,
                          const OmSegID value);

// segment-related
    static void ValidateSegment(const SegmentDataWrapper& sdw,
                                const om::SetValid valid, const bool dontCenter=false);

    static void ValidateSelectedSegments(const SegmentationDataWrapper& sdw,
                                         const om::SetValid valid);

    static void UncertainSegment(const SegmentDataWrapper& sdw,
                                 const bool uncertain);

    static void UncertainSegmentation(const SegmentationDataWrapper& sdw,
                                 const bool uncertain);

    static void JoinSegments(const OmID segmentationID,
                             const OmSegIDsSet& ids);

    static void JoinSegments(const OmID segmentationID);
    static void JoinSegments(const SegmentationDataWrapper& sdw);

    static void FindAndSplitSegments(const SegmentDataWrapper& sdw,
                                     OmViewGroupState* vgs, const DataCoord coord);
    static void FindAndCutSegments(const SegmentDataWrapper& sdw,
                                   OmViewGroupState* vgs);

    static void SelectSegments(boost::shared_ptr<OmSelectSegmentsParams> params);

// group-related
    static void CreateOrDeleteSegmentGroup(const OmID segmentationID,
                                           const OmSegIDsSet& selectedSegmentIDs,
                                           const OmGroupName name,
                                           const bool create);
private:
    OmActions(){}
    ~OmActions(){}

    friend class zi::singleton<OmActions>;
};

#endif
