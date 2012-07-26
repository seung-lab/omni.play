#pragma once

#include "common/omCommon.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "threads/omTaskManagerTypes.h"
#include "segment/io/omMSTtypes.h"

// #define OMSEGMENTGRAPH_NEWLEVEL -1
// #define STEP 100
// #define MAX_NOS 10

//using namespace std;

class OmSegmentSelector;
class OmMST;
class OmSegmentsStore;
class OmSegment;
class OmSegmentsImplLowLevel;
class OmSegmentChildren;
class OmSegmentListLowLevel;
class OmSegmentation;
class OmSegmentSelection;
class OmValidGroupNum;


class OmSegmentGraph {
public:
    static const int omsegmentgraph_newlevel = -1 ;
    static const int step                    = 100;
    static const int max_nos                 = 10 ;

    typedef std::vector<OmSegment*> some_type[max_nos][max_nos*2+2][max_nos];

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
    void ResetSizeThreshold(OmMST* mst);
    void ResetSizeThresholdUp(OmMST* mst);
    void ResetSizeThresholdDown(OmMST* mst);

    double SizeOfBFSGrowth(OmMST* mst, OmSegID SegmentID, double globalThreshold);
    void Grow_LocalSizeThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID);
    void AddSegments_BreadthFirstSearch(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID);
    void AddSegments_BFS_DynamicThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID);
    void Trim(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID);

    void UpdateSizeListsFromJoin(OmSegment* root, OmSegment* child);
    void UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child);

    OmSegmentChildren* Children(){
        return children_.get();
    }

    typedef boost::unordered_map < OmSegID, std::vector <OmMSTEdge*> > AdjacencyMap;

    inline AdjacencyMap* GetAdjacencyList()
    {
        return &adjacencyList_;
    }

    inline boost::unordered_map <OmSegID,uint32_t>* GetOrderOfAdding()
    {
        return &orderOfAdding;
    }

    bool joinInternal(const OmSegID parentID,
                      const OmSegID childUnknownDepthID,
                      const double threshold,
                      const int edgeNumber);

    bool splitChildFromParentInternal(const OmSegID childID);

private:
    OmSegmentation* segmentation_;
    OmValidGroupNum* validGroupNum_;
    OmSegmentsImplLowLevel* mCache;
    OmSegmentsStore* segmentPages_;
    AdjacencyMap adjacencyList_;
    boost::unordered_map <OmSegID,uint32_t> orderOfAdding;
    some_type distribution_;
    boost::unordered_map <OmSegID,OmSegment*> accessToSegments_;
    boost::scoped_ptr<OmDynamicForestCache> forest_;
    boost::scoped_ptr<OmSegmentChildren> children_;
    OmSegmentListLowLevel* segmentListsLL_;

    SizeAndNumPieces computeSegmentSizeWithChildren(OmSegment* seg);
    std::vector<OmSegment*> segsTempVec_;
    
    bool sizeCheck(const OmSegID a, const OmSegID b, const double threshold);
};

