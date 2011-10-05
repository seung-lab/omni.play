#pragma once

#include "common/common.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "threads/omTaskManagerTypes.h"

class OmMST;
class segmentsStore;
class OmSegment;
class segmentsImplLowLevel;
class OmSegmentChildren;
class OmSegmentListLowLevel;
class segmentation;
class OmValidGroupNum;

class OmSegmentGraph {
public:
    OmSegmentGraph();
    ~OmSegmentGraph();

    void Initialize(segmentation* segmentation,
                    segmentsImplLowLevel* cache);
    bool DoesGraphNeedToBeRefreshed(const uint32_t maxValue);
    void GrowGraphIfNeeded(OmSegment* newSeg);

    void RefreshGUIlists();

    inline uint64_t MSTfreshness() const {
        return forest_->Freshness();
    }
    inline segId Root(const segId segID){
        return forest_->Root(segID);
    }
    inline void Cut(const segId segID){
        forest_->Cut(segID);
    }
    inline void Join(const segId childRootID, const segId parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    void SetGlobalThreshold(OmMST* mst);
    void ResetGlobalThreshold(OmMST* mst);

    void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child);
    void UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child);

    OmSegmentChildren* Children(){
        return children_.get();
    }

private:
    segmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    segmentsImplLowLevel* mCache;
    segmentsStore* segmentPages_;

    boost::scoped_ptr<OmDynamicForestCache> forest_;
    boost::scoped_ptr<OmSegmentChildren> children_;
    OmSegmentListLowLevel* segmentListsLL_;

    bool joinInternal(const segId parentID,
                      const segId childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const segId childID);

    SizeAndNumPieces computeSegmentSizeWithChildren(OmSegment* seg);
    std::vector<OmSegment*> segsTempVec_;
    
    bool sizeCheck(const segId a, const segId b, const double threshold);
};

