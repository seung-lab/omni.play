#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"

class OmMST;
class OmSegmentsStore;
class OmSegment;
class OmSegmentsImplLowLevel;
class OmSegmentChildren;
class OmSegmentListLowLevel;
class OmSegmentation;
class OmValidGroupNum;
class OmThreadPool;

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
    inline OmSegID Root(const OmSegID segID){
        return forest_->Root(segID);
    }
    inline void Cut(const OmSegID segID){
        forest_->Cut(segID);
    }
    inline void Join(const OmSegID childRootID, const OmSegID parentRootID){
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
    OmSegmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    OmSegmentsImplLowLevel* mCache;
    OmSegmentsStore* segmentPages_;

    boost::scoped_ptr<OmDynamicForestCache> forest_;
    boost::scoped_ptr<OmSegmentChildren> children_;
    OmSegmentListLowLevel* segmentListsLL_;

    void buildSegmentSizeLists();

    bool joinInternal(const OmSegID parentID,
                      const OmSegID childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const OmSegID childID);

    SizeAndNumPieces computeSegmentSizeWithChildren(OmSegment* seg);
    std::vector<OmSegment*> segsTempVec_;
};

#endif
