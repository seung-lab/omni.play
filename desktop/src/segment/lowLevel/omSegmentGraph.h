#pragma once

#include "common/common.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "threads/omTaskManagerTypes.h"

class OmMST;
class OmSegmentsStore;
class OmSegment;
class OmSegmentsImplLowLevel;
class OmSegmentChildren;
class OmSegmentListLowLevel;
class OmSegmentation;
class OmValidGroupNum;

class OmSegmentGraph {
public:
    OmSegmentGraph();
    ~OmSegmentGraph();

    void Initialize(OmSegmentation* segmentation,
                    OmSegmentsImplLowLevel* cache);
    bool DoesGraphNeedToBeRefreshed(const uint32_t maxValue);
    void GrowGraphIfNeeded(OmSegment* newSeg);

    void RefreshGUIlists();

    inline uint64_t MSTfreshness() const {
        return forest_->Freshness();
    }
    inline om::common::SegID Root(const om::common::SegID segID){
        return forest_->Root(segID);
    }
    inline void Cut(const om::common::SegID segID){
        forest_->Cut(segID);
    }
    inline void Join(const om::common::SegID childRootID, const om::common::SegID parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    void SetGlobalThreshold(OmMST* mst);
    void ResetGlobalThreshold(OmMST* mst);

    void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child);
    void UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child);

    OmSegmentChildren* Children(){
        return children_.get();
    }

    //Do stuff

private:
    OmSegmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    OmSegmentsImplLowLevel* mCache;
    OmSegmentsStore* segmentPages_;

    std::unique_ptr<OmDynamicForestCache> forest_;
    std::unique_ptr<OmSegmentChildren> children_;
    OmSegmentListLowLevel* segmentListsLL_;

    bool joinInternal(const om::common::SegID parentID,
                      const om::common::SegID childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const om::common::SegID childID);

    SizeAndNumPieces computeSegmentSizeWithChildren(OmSegment* seg);
    std::vector<OmSegment*> segsTempVec_;
    
    bool sizeCheck(const om::common::SegID a, const om::common::SegID b, const double threshold);
};

