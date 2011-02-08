#include "segment/lowLevel/omSegmentBags.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/io/omMST.h"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "utility/omTimer.hpp"

OmSegmentGraph::OmSegmentGraph()
    : segmentation_(NULL)
    , mCache(NULL)
{}

OmSegmentGraph::~OmSegmentGraph()
{}

bool OmSegmentGraph::DoesGraphNeedToBeRefreshed( const uint32_t maxValue )
{
    return !forest_ || forest_->Size() != maxValue+1;
}

void OmSegmentGraph::Initialize(OmSegmentation* segmentation,
                                OmSegmentCacheImplLowLevel* cache)
{
    segmentation_ = segmentation;
    validGroupNum_ = segmentation->ValidGroupNum();
    segmentLists_ = segmentation->SegmentLists();
    mCache = cache;

    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    bags_.reset(new OmSegmentBags(size));
    forest_.reset(new OmDynamicForestCache(size));

    buildSegmentSizeLists();
}

void OmSegmentGraph::GrowGraphIfNeeded(OmSegment* seg)
{
    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();
    forest_->Resize(size);
    bags_->Resize(size);
    segmentLists_->InsertSegmentWorking(seg);
}

void OmSegmentGraph::buildSegmentSizeLists()
{
    segmentLists_->Clear();

    OmSegmentIteratorLowLevel iter(mCache);
    iter.iterOverAllSegments();

    for(OmSegment* seg = iter.getNextSegment();
        NULL != seg;
        seg = iter.getNextSegment())
    {
        if(!seg->getParent()) {
            switch(seg->GetListType()){
            case om::WORKING:
                segmentLists_->InsertSegmentWorking(seg);
                break;
            case om::VALID:
                segmentLists_->InsertSegmentValid(seg);
                break;
            case om::UNCERTAIN:
                segmentLists_->InsertSegmentUncertain(seg);
                break;
            default:
                throw OmArgException("unsupprted list arg");
            }
        }
    }
}

uint32_t OmSegmentGraph::GetNumTopLevelSegs()
{
    return segmentLists_->GetNumTopLevelSegs();
}

void OmSegmentGraph::SetGlobalThreshold(OmMST* mst)
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
            1 == edges[i].userJoin){ // join
            if( 1 == edges[i].wasJoined ){
                continue;
            }

            if( joinInternal( edges[i].node2ID,
                              edges[i].node1ID,
                              edges[i].threshold,
                              i) ){
                edges[i].wasJoined = 1;
            } else {
                edges[i].userSplit = 1;
            }
        }
    }

    forest_->SetBatch(false);

    printf("done (%f secs)\n", timer.s_elapsed() );
}

void OmSegmentGraph::ResetGlobalThreshold(OmMST* mst)
{
    std::cout << "\t" << om::string::humanizeNum(mst->NumEdges())
              << " edges..." << std::flush;

    forest_->SetBatch(true);
    forest_->ClearCache();

    const double stopThreshold = mst->UserThreshold();
    OmMSTEdge* edges = mst->Edges();

    for(uint32_t i = 0; i < mst->NumEdges(); ++i) {
        if( 1 == edges[i].userSplit ){
            continue;
        }

        if( edges[i].threshold >= stopThreshold ||
            1 == edges[i].userJoin ){ // join
            if( 1 == edges[i].wasJoined ){
                continue;
            }
            if( joinInternal(edges[i].node2ID,
                             edges[i].node1ID,
                             edges[i].threshold, i) ){
                edges[i].wasJoined = 1;
            } else {
                edges[i].userSplit = 1;
            }
        } else { // split
            if( 0 == edges[i].wasJoined ){
                continue;
            }
            if( splitChildFromParentInternal(edges[i].node1ID)){
                edges[i].wasJoined = 0;
            } else {
                edges[i].userJoin = 1;
            }
        }
    }

    forest_->SetBatch(false);

    printf("done\n");
}

bool OmSegmentGraph::joinInternal( const OmSegID parentID,
                                   const OmSegID childUnknownDepthID,
                                   const double threshold,
                                   const int edgeNumber )
{
    const OmSegID childRootID = Root(childUnknownDepthID);
    const OmSegID parentRootID = Root(parentID);
    OmSegment* childRoot = mCache->GetSegment(childRootID);
    OmSegment* parent = mCache->GetSegment( parentID );

    if(childRootID == parentRootID){
        return false;
    }

    if(childRoot->IsValidListType() != parent->IsValidListType()){
        return false;
    }

    if(childRoot->IsValidListType()){
        if(validGroupNum_->Get(childRootID) !=
           validGroupNum_->Get(parentID))
        {
            return false;
        }
    }

    Join(childRootID, parentID);

    parent->addChild(childRoot);
    childRoot->setParent(parent, threshold);
    childRoot->setEdgeNumber(edgeNumber);

    mCache->FindRoot(parent)->touchFreshnessForMeshes();

    UpdateSizeListsFromJoin( parent, childRoot );

    return true;
}

bool OmSegmentGraph::splitChildFromParentInternal( const OmSegID childID )
{
    OmSegment* child = mCache->GetSegment( childID );

    if( child->getThreshold() > 1 ){
        return false;
    }

    OmSegment* parent = child->getParent();
    if(!parent){ // user manually split?
        return false;
    }

    if( child->IsValidListType() || parent->IsValidListType()){
        return false;
    }

    parent->removeChild(child);
    Cut(child->value());
    child->setParent(NULL); // TODO: also set threshold??
    child->setEdgeNumber(-1);

    OmSegment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();
    child->touchFreshnessForMeshes();

    UpdateSizeListsFromSplit(parentRoot, child);

    return true;
}

void OmSegmentGraph::UpdateSizeListsFromJoin(OmSegment* parent, OmSegment* child)
{
    OmSegment* root = mCache->FindRoot(parent);
    segmentLists_->UpdateFromJoinWorking(root, child);
    segmentLists_->UpdateFromJoinUncertain(root, child);
    segmentLists_->UpdateFromJoinValid(root, child);

    //bags_->Join(root, child);
}

void OmSegmentGraph::UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child)
{
    OmSegment* root = mCache->FindRoot(parent);
    uint64_t newChildSize = computeSegmentSizeWithChildren(child->value());
    segmentLists_->UpdateFromSplitWorking(root, child, newChildSize);
    segmentLists_->UpdateFromSplitUncertain(root, child, newChildSize);
//	segmentLists_->UpdateFromSplitValid(root, child, newChildSize);

    //bags_->Split(root, child);
}

quint64 OmSegmentGraph::computeSegmentSizeWithChildren( const OmSegID segID )
{
    quint64 size = 0;
    OmSegmentIteratorLowLevel iter(mCache);
    iter.iterOverSegmentID( segID );
    for(OmSegment* seg = iter.getNextSegment(); NULL != seg; seg = iter.getNextSegment()){
        size += seg->size();
    }
    return size;
}
