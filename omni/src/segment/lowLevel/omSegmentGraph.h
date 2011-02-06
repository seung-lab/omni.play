#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "common/omCommon.h"

class OmSegmentation;
class OmSegmentBags;
class OmSegment;
class OmSegmentCacheImplLowLevel;
class OmSegmentLists;
class OmMST;
class OmValidGroupNum;

class OmSegmentGraph {
public:
    OmSegmentGraph();
    ~OmSegmentGraph();

    void Initialize(OmSegmentation* segmentation,
                    OmSegmentCacheImplLowLevel* cache);
    bool DoesGraphNeedToBeRefreshed(const uint32_t maxValue);
    void GrowGraphIfNeeded(OmSegment* newSeg);

    inline OmSegID Root(const OmSegID segID){
        return forest_->Root(segID);
    }
    inline void Cut(const OmSegID segID){
        forest_->Cut(segID);
    }
    inline void Join(const OmSegID childRootID, const OmSegID parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    uint32_t GetNumTopLevelSegs();

    void SetGlobalThreshold(OmMST* mst);
    void ResetGlobalThreshold(OmMST* mst);

    void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child);
    void UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child);

private:
    OmSegmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    OmSegmentLists* segmentLists_;
    OmSegmentCacheImplLowLevel* mCache;

    boost::scoped_ptr<OmDynamicForestCache> forest_;
    boost::scoped_ptr<OmSegmentBags> bags_;

    void buildSegmentSizeLists();

    bool joinInternal(const OmSegID parentID,
                      const OmSegID childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const OmSegID childID);

    quint64 computeSegmentSizeWithChildren(const OmSegID segID);

};

#endif
