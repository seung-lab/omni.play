#pragma once

#include "common/common.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/segmentLowLevelTypes.h"
#include "threads/omTaskManagerTypes.h"

class OmMST;
class segmentsStore;
class segment;
class segmentsImplLowLevel;
class segmentChildren;
class segmentListLowLevel;
class segmentation;
class OmValidGroupNum;

class segmentGraph {
public:
    segmentGraph();
    ~segmentGraph();

    void Initialize(segmentation* segmentation,
                    segmentsImplLowLevel* cache);
    bool DoesGraphNeedToBeRefreshed(const uint32_t maxValue);
    void GrowGraphIfNeeded(segment* newSeg);

    void RefreshGUIlists();

    inline uint64_t MSTfreshness() const {
        return forest_->Freshness();
    }
    inline common::segId Root(const common::segId segID){
        return forest_->Root(segID);
    }
    inline void Cut(const common::segId segID){
        forest_->Cut(segID);
    }
    inline void Join(const common::segId childRootID, const common::segId parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    void SetGlobalThreshold(OmMST* mst);
    void ResetGlobalThreshold(OmMST* mst);

    void UpdateSizeListsFromJoin(segment* root, segment* child);
    void UpdateSizeListsFromSplit(segment* parent, segment* child);

    segmentChildren* Children(){
        return children_.get();
    }

private:
    segmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    segmentsImplLowLevel* mCache;
    segmentsStore* segmentPages_;

    boost::scoped_ptr<dynamicForestCache> forest_;
    boost::scoped_ptr<segmentChildren> children_;
    segmentListLowLevel* segmentListsLL_;

    bool joinInternal(const common::segId parentID,
                      const common::segId childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const common::segId childID);

    SizeAndNumPieces computeSegmentSizeWithChildren(segment* seg);
    std::vector<segment*> segsTempVec_;
    
    bool sizeCheck(const common::segId a, const common::segId b, const double threshold);
};

