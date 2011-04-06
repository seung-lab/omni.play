#include "segment/lowLevel/omSegmentGraphInitialLoad.hpp"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"

OmSegmentGraph::OmSegmentGraph()
    : segmentation_(NULL)
    , mCache(NULL)
    , segmentPages_(NULL)
    , segmentListsLL_(NULL)
{
    segsTempVec_.reserve(100);
}

OmSegmentGraph::~OmSegmentGraph()
{}

void OmSegmentGraph::RefreshGUIlists(){
    segmentListsLL_->RefreshGUIlists();
}

bool OmSegmentGraph::DoesGraphNeedToBeRefreshed( const uint32_t maxValue )
{
    return !forest_ || forest_->Size() != maxValue+1;
}

void OmSegmentGraph::Initialize(OmSegmentation* segmentation,
                                OmSegmentsImplLowLevel* cache)
{
    segmentation_ = segmentation;
    validGroupNum_ = segmentation->ValidGroupNum();
    mCache = cache;
    segmentPages_ = cache->SegmentStore();

    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    // DynamicForestPool dies if size == 1 during root()
    if(size <= 10){
        forest_.reset(new OmDynamicForestCache(10));
    } else {
        forest_.reset(new OmDynamicForestCache(size));
    }

    children_.reset(new OmSegmentChildren(size));

    validGroupNum_->Resize(size);

    segmentListsLL_ = segmentation->SegmentLists()->LowLevelList();
    segmentListsLL_->SetCache(mCache);
    segmentListsLL_->Resize(size);

    buildSegmentSizeLists();
}

void OmSegmentGraph::GrowGraphIfNeeded(OmSegment* seg)
{
    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    // DynamicForestPool dies if newSize == oldSize during resize()
    if(size > 10){
        forest_->Resize(size);
    }

    children_->Resize(size);
    segmentListsLL_->Resize(size);
    segmentListsLL_->AddSegment(seg);
    segmentListsLL_->ForceRefreshGUIlists();
}

void OmSegmentGraph::buildSegmentSizeLists() {
    segmentListsLL_->BuildInitialSegmentList();
}

void OmSegmentGraph::SetGlobalThreshold(OmMST* mst)
{
    OmSegmentGraphInitialLoad loader(forest_.get(),
                                     validGroupNum_,
                                     segmentListsLL_,
                                     segmentPages_,
                                     children_.get());

    loader.SetGlobalThreshold(mst);
}

void OmSegmentGraph::ResetGlobalThreshold(OmMST* mst)
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
            1 == edges[i].userJoin )
        { // join
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

    timer.PrintDone();
}

bool OmSegmentGraph::joinInternal(const OmSegID parentID,
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

    OmSegment* childRoot = segmentPages_->GetSegmentUnsafe(childRootID);
    OmSegment* parent = segmentPages_->GetSegmentUnsafe(parentID);

    children_->AddChild(parent, childRoot);
    childRoot->setParent(parent, threshold);
    childRoot->setEdgeNumber(edgeNumber);

    OmSegment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();

    //UpdateSizeListsFromJoin(parentRoot, childRoot );
    segmentListsLL_->UpdateSizeListsFromJoin(parentRoot, childRoot);

    return true;
}


bool OmSegmentGraph::splitChildFromParentInternal( const OmSegID childID )
{
    OmSegment* child = mCache->SegmentStore()->GetSegment( childID );

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

    children_->RemoveChild(parent, child);
    Cut(childID);
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
    segmentListsLL_->UpdateSizeListsFromJoin(root, child);
}

void OmSegmentGraph::UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child)
{
    OmSegment* root = mCache->FindRoot(parent);

    const SizeAndNumPieces childInfo =
        computeSegmentSizeWithChildren(child);

    segmentListsLL_->UpdateSizeListsFromSplit(root, child, childInfo);
}

SizeAndNumPieces
OmSegmentGraph::computeSegmentSizeWithChildren(OmSegment* inSeg)
{
    int64_t numVoxels = 0;
    int32_t numPieces = 0;

    segsTempVec_.push_back(inSeg);

    while(!segsTempVec_.empty())
    {
        OmSegment* segRet = segsTempVec_.back();
        segsTempVec_.pop_back();

        FOR_EACH(iter, children_->GetChildren(segRet)){
            segsTempVec_.push_back(*iter);
        }

        const int64_t segSize = segRet->size();

        assert(segSize > 0);

        numVoxels += segSize;

        assert(numVoxels > 0);

        ++numPieces;
    }

    SizeAndNumPieces ret = { numVoxels, numPieces };
    return ret;
}
