#pragma once

#include "segment/io/omMST.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentListLowLevel.hpp"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"
#include "threads/omThreadPoolBatched.hpp"
#include "utility/omTimer.hpp"
#include "segment/io/omMSTtypes.h"
#include <boost/unordered_map.hpp>

//#include "../../../../common/src/coordinates/chunk.h"
//#include "../../../../common/src/coordinates/data.h"

uint32_t numberOfAddedSegment;

//using namespace std;

class chunk;
class data;

class OmSegmentGraphInitialLoad {
private:
    static const int step                    = 100;
    static const int max_nos                 = 10 ;

    OmDynamicForestCache *const forest_;
    OmValidGroupNum *const validGroupNum_;
    OmSegmentListLowLevel *const segmentListsLL_;
    OmSegmentsStore *const segmentPages_;
    OmSegmentChildren *const children_;
    OmSegmentGraph::AdjacencyMap *const AdjacencyList_;
    boost::unordered_map <OmSegID,uint32_t> *const orderOfAdding_;
    OmSegmentGraph::some_type* distribution_;
    boost::unordered_map <OmSegID,OmSegment*> *const accessToSegments_;

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
                              OmSegmentListLowLevel* segmentListLL,
                              OmSegmentsStore* segmentPages,
                              OmSegmentChildren* children,
                              OmSegmentGraph::AdjacencyMap *AdjacencyList,
                              boost::unordered_map <OmSegID,uint32_t> *orderOfAdding,
                              OmSegmentGraph::some_type* distribution,
                              boost::unordered_map <OmSegID,OmSegment*> *accessToSegments)
        : forest_(forest)
        , validGroupNum_(validGroupNum)
        , segmentListsLL_(segmentListLL)
        , segmentPages_(segmentPages)
        , children_(children)
        , AdjacencyList_(AdjacencyList)
        , orderOfAdding_(orderOfAdding)
        , distribution_(distribution)
        , accessToSegments_(accessToSegments)
    {

        joinTaskPool_.Start(&OmSegmentGraphInitialLoad::initialJoinInternalTask,
                            this,
                            1);
    }

    struct point
    {
        int x,y,z;
    };

    inline int abss(int a)
    {
        if ( a < 0 ) return -a;
        return a;
    }

    void DistributeSegmentsInBatches(OmMST *mst)
    {
        OmMSTEdge* edges = mst->Edges();

        OmSegment *segment,*segment1,*segment2;
        OmSegID segID;

        point center_,batch_,center_1,center_2,batch_1,batch_2;
        int i,j;
        for ( i=0; i<segmentListsLL_->GetList()->size(); i++ )
        {
            segment = (* segmentListsLL_->GetList() )[i].seg;
            if ( segment == NULL ) continue;

            segID = segment->GetData()->value;
            (*accessToSegments_)[segID] = segment;

            center_.x = segment->GetData()->bounds.getCenter().x; // Extracting the center of the current segment
            center_.y = segment->GetData()->bounds.getCenter().y;
            center_.z = segment->GetData()->bounds.getCenter().z;

            batch_.x = center_.x/step + (center_.x%step)?1:0; // Determine which batch the current segment belongs to
            batch_.y = center_.y/step + (center_.y%step)?1:0;
            batch_.z = center_.z/step + (center_.z%step)?1:0;
            (*distribution_)[ batch_.x ][ batch_.y ][ batch_.z ].push_back(segment);
        }

        /*for ( it.x = step; it.x <= volumeSize; it.x += step ) ----- Traversing the distribution array
            for ( it.y = step; it.y <= volumeSize; it.y += step )
                for ( it.z = step; it.z <= volumeSize; it.z += step )
                {

                }*/


        /*for ( i = 0; i < mst->NumEdges(); ++i ) ------ Check if edges are only between neighbouring batches - yep for now
        {
            segment1 = (*accessToSegments_)[ edges[i].node1ID ];
            segment2 = (*accessToSegments_)[ edges[i].node2ID ];
            
            center_1.x = segment1->GetData()->bounds.getCenter().x;
            center_1.y = segment1->GetData()->bounds.getCenter().y;
            center_1.z = segment1->GetData()->bounds.getCenter().z;

            batch_1.x = center_1.x/STEP + (center_1.x%STEP)?1:0;
            batch_1.y = center_1.y/STEP + (center_1.y%STEP)?1:0;
            batch_1.z = center_1.z/STEP + (center_1.z%STEP)?1:0;
            
            center_2.x = segment2->GetData()->bounds.getCenter().x;
            center_2.y = segment2->GetData()->bounds.getCenter().y;
            center_2.z = segment2->GetData()->bounds.getCenter().z;

            batch_2.x = center_2.x/STEP + (center_2.x%STEP)?1:0;
            batch_2.y = center_2.y/STEP + (center_2.y%STEP)?1:0;
            batch_2.z = center_2.z/STEP + (center_2.z%STEP)?1:0;

            if ( abss( batch_1.x - batch_2.x ) + abss( batch_1.y - batch_2.y ) + abss( batch_1.z - batch_2.z ) > 1 )
            {
                std::cout << batch_1.x << ' ' << batch_1.y << ' ' << batch_1.z << std::endl;
                std::cout << batch_2.x << ' ' << batch_2.y << ' ' << batch_2.z << std::endl << std::endl;
            }
        }*/
    }

    void SetGlobalThreshold(OmMST* mst)
    {
        numberOfAddedSegment = 0;

        std::cout << "\t" << om::string::humanizeNum(mst->NumEdges())
                  << " edges..." << std::flush;

        OmTimer timer;

        forest_->SetBatch(true);
        forest_->ClearCache();

        const double stopThreshold = mst->UserThreshold();
        OmMSTEdge* edges = mst->Edges();

        for(uint32_t i = 0; i < mst->NumEdges(); ++i) 
        {
            (*AdjacencyList_)[edges[i].node1ID].push_back(&edges[i]);
            (*AdjacencyList_)[edges[i].node2ID].push_back(&edges[i]);

            //AllSegIDs.insert(edges[i].node1ID);
            //AllSegIDs.insert(edges[i].node2ID); 

            if( 1 == edges[i].userSplit ){
                continue;
            }

            if(edges[i].threshold >= stopThreshold ||
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

        std::vector<SegInfo> ::iterator it;
        std::vector <SegInfo>* listOfSegments = (*segmentListsLL_).GetList();

        for ( it = listOfSegments->begin(); it != listOfSegments->end(); it++ )
        {
            (*orderOfAdding_)[ (*it).segID ] = 0;
            sort( (*AdjacencyList_)[ (*it).segID ].begin(), (*AdjacencyList_)[ (*it).segID ].end());
        }

        joinTaskPool_.JoinPool();

        forest_->SetBatch(false);

        timer.PrintDone();
        
        DistributeSegmentsInBatches(mst);
    }

private:

    inline OmSegID Root(const OmSegID segID){
        return forest_->Root(segID);
    }

    inline void Join(const OmSegID childRootID, const OmSegID parentRootID){
        forest_->Join(childRootID, parentRootID);
    }

    bool sizeCheck(const OmSegID a, const OmSegID b, const double threshold)
    {
        return threshold == 0 ||
               ((segmentListsLL_->GetSizeWithChildren(Root(a)) +
                 segmentListsLL_->GetSizeWithChildren(Root(b))) < threshold);
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

