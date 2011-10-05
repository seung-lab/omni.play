#pragma once

#include "segment/io/omMST.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentListLowLevel.hpp"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"
#include "threads/omThreadPoolBatched.hpp"
#include "utility/omTimer.hpp"

class OmSegmentGraphInitialLoad {
private:
    OmDynamicForestCache *const forest_;
    OmValidGroupNum *const validGroupNum_;
    OmSegmentListLowLevel *const segmentListsLL_;
    segmentsStore *const segmentPages_;
    OmSegmentChildren *const children_;

    OmThreadPool pool_;

    struct TaskArgs {
        segId childRootID;
        segId parentID;
        segId parentRootID;
        double threshold;
        int edgeNumber;
    };

    OmThreadPoolBatched<TaskArgs,
                        OmSegmentGraphInitialLoad,
                        IndivArgPolicy> joinTaskPool_;

public:
    OmSegmentGraphInitialLoad(OmDynamicForestCache* forest,
                              OmValidGroupNum* validGroupNum,
                              OmSegmentListLowLevel* segmentListLL,
                              segmentsStore* segmentPages,
                              OmSegmentChildren* children)
        : forest_(forest)
        , validGroupNum_(validGroupNum)
        , segmentListsLL_(segmentListLL)
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
        const double sizeThreshold = mst->UserSizeThreshold();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < mst->NumEdges(); ++i) {
            if( 1 == edges[i].userSplit ){
                continue;
            }
            
            if( (edges[i].threshold >= stopThreshold && // prob threshold
                sizeCheck(edges[i].node1ID, edges[i].node2ID, sizeThreshold)) || // size threshold
                1 == edges[i].userJoin )
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

    inline segId Root(const segId segID){
        return forest_->Root(segID);
    }

    inline void Join(const segId childRootID, const segId parentRootID){
        forest_->Join(childRootID, parentRootID);
    }
    
    bool sizeCheck(const segId a, const segId b, const double threshold)
    {
        return (segmentListsLL_->GetSizeWithChildren(Root(a)) + 
                segmentListsLL_->GetSizeWithChildren(Root(b))) < threshold;
    }

    bool initialJoinInternal(const segId parentID,
                             const segId childUnknownDepthID,
                             const double threshold,
                             const int edgeNumber)
    {
        const segId childRootID = Root(childUnknownDepthID);
        const segId parentRootID = Root(parentID);

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

