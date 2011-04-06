#ifndef OM_SEGMENT_GRAPH_INITIAL_LOAD_HPP
#define OM_SEGMENT_GRAPH_INITIAL_LOAD_HPP

#include "segment/io/omMST.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentListLowLevel.hpp"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"
#include "utility/omThreadPoolBatched.hpp"
#include "utility/omTimer.hpp"

class OmSegmentGraphInitialLoad {
private:
    OmDynamicForestCache *const forest_;
    OmValidGroupNum *const validGroupNum_;
    OmSegmentListLowLevel *const segmentListsLL_;
    OmSegmentsStore *const segmentPages_;
    OmSegmentChildren *const children_;

    OmThreadPool pool_;

    struct TaskArgs {
        OmSegID childRootID;
        OmSegID parentID;
        OmSegID parentRootID;
        double threshold;
        int edgeNumber;
    };

    OmThreadPoolBatched<TaskArgs,
                        OmSegmentGraphInitialLoad,
                        IndivArgPolicy> joinTaskPool_;

public:
    OmSegmentGraphInitialLoad(OmDynamicForestCache* forest,
                              OmValidGroupNum* validGroupNum,
                              OmSegmentListLowLevel* segmentListsLL,
                              OmSegmentsStore* segmentPages,
                              OmSegmentChildren* children)
        : forest_(forest)
        , validGroupNum_(validGroupNum)
        , segmentListsLL_(segmentListsLL)
        , segmentPages_(segmentPages)
        , children_(children)
    {
        joinTaskPool_.Start(&OmSegmentGraphInitialLoad::initialJoinInternalTask,
                            this,
                            1);
    }

    void SetGlobalThreshold(OmMST* mst)
    {
        std::cout << "\t" << om::string::humanizeNum(mst->NumEdges())
                  << " edges..." << std::flush;

        OmTimer timer;

        forest_->SetBatch(true);
        forest_->ClearCache();

        const double stopThreshold = mst->UserThreshold();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < mst->NumEdges(); ++i) {
            if( 1 == edges[i].userSplit ){
                continue;
            }

            if( edges[i].threshold >= stopThreshold ||
                1 == edges[i].userJoin)
            { // join
                if( 1 == edges[i].wasJoined ){
                    continue;
                }

                if(initialJoinInternal( edges[i].node2ID,
                                        edges[i].node1ID,
                                        edges[i].threshold,
                                        i) )
                {
                    edges[i].wasJoined = 1;
                } else {
                    edges[i].userSplit = 1;
                }
            }
        }

        joinTaskPool_.JoinPool();

        forest_->SetBatch(false);

        timer.PrintDone();
    }

private:

    inline OmSegID Root(const OmSegID segID){
        return forest_->Root(segID);
    }

    inline void Join(const OmSegID childRootID, const OmSegID parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    bool initialJoinInternal(const OmSegID parentID,
                             const OmSegID childUnknownDepthID,
                             const double threshold,
                             const int edgeNumber)
    {
        const OmSegID childRootID = Root(childUnknownDepthID);
        const OmSegID parentRootID = Root(parentID);

        if(childRootID == parentRootID){
            return false;
        }

        if(!validGroupNum_->InSameValidGroup(childRootID, parentID)){
            return false;
        }

        Join(childRootID, parentID);

        TaskArgs t = { childRootID,
                       parentID,
                       parentRootID,
                       threshold,
                       edgeNumber };

        joinTaskPool_.AddOrSpawnTasks(t);

        return true;
    }

    void initialJoinInternalTask(const TaskArgs& t)
    {
        OmSegment* childRoot = segmentPages_->GetSegmentUnsafe(t.childRootID);
        OmSegment* parent = segmentPages_->GetSegmentUnsafe(t.parentID);

        children_->AddChild(t.parentID, childRoot);
        childRoot->setParent(parent, t.threshold);
        childRoot->setEdgeNumber(t.edgeNumber);

        OmSegment* parentRoot = parent;
        if(t.parentRootID != t.parentID){
            parentRoot = segmentPages_->GetSegmentUnsafe(t.parentRootID);
        }

        segmentListsLL_->UpdateSizeListsFromJoin(parentRoot, childRoot);
    }
};

#endif
