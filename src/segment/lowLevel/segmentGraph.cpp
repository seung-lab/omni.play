#include "segment/lowLevel/segmentGraphInitialLoad.hpp"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "segment/lowLevel/segmentGraph.h"
#include "utility/omStringHelpers.h"
#include "volume/segmentation.h"

segmentGraph::segmentGraph()
    : segmentation_(NULL)
    , mCache(NULL)
    , segmentPages_(NULL)
    , segmentListsLL_(NULL)
{
    segsTempVec_.reserve(100);
}

segmentGraph::~segmentGraph()
{}

void segmentGraph::RefreshGUIlists(){
    segmentListsLL_->RefreshGUIlists();
}

bool segmentGraph::DoesGraphNeedToBeRefreshed( const uint32_t maxValue )
{
    return !forest_ || forest_->Size() != maxValue+1;
}

void segmentGraph::Initialize(segmentation* segmentation,
                                segmentsImplLowLevel* cache)
{
    segmentation_ = segmentation;
    validGroupNum_ = segmentation->ValidGroupNum();
    mCache = cache;
    segmentPages_ = cache->SegmentStore();

    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    forest_.reset(new dynamicForestCache(size));
    children_.reset(new segmentChildren(size));

    validGroupNum_->Resize(size);

    segmentListsLL_ = segmentation->SegmentLists()->LowLevelList();
    segmentListsLL_->SetCache(mCache);
    segmentListsLL_->Resize(size);

    segmentListsLL_->BuildInitialSegmentList();
}

void segmentGraph::GrowGraphIfNeeded(segment* seg)
{
    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    forest_->Resize(size);
    children_->Resize(size);
    segmentListsLL_->Resize(size);
    segmentListsLL_->AddSegment(seg);
    segmentListsLL_->ForceRefreshGUIlists();
}

void segmentGraph::SetGlobalThreshold(OmMST* mst)
{
    segmentGraphInitialLoad loader(forest_.get(),
                                     validGroupNum_,
                                     segmentListsLL_,
                                     segmentPages_,
                                     children_.get());

    loader.SetGlobalThreshold(mst);
}

void segmentGraph::ResetGlobalThreshold(OmMST* mst)
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

bool segmentGraph::sizeCheck(const segId a, const segId b, const double threshold)
{
    return (segmentListsLL_->GetSizeWithChildren(Root(a)) + 
            segmentListsLL_->GetSizeWithChildren(Root(b))) < threshold;
}

bool segmentGraph::joinInternal(const segId parentID,
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

    segment* childRoot = segmentPages_->GetSegmentUnsafe(childRootID);
    segment* parent = segmentPages_->GetSegmentUnsafe(parentID);

    children_->AddChild(parent, childRoot);
    childRoot->setParent(parent, threshold);
    childRoot->setEdgeNumber(edgeNumber);

    segment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();

    //UpdateSizeListsFromJoin(parentRoot, childRoot );
    segmentListsLL_->UpdateSizeListsFromJoin(parentRoot, childRoot);

    return true;
}


bool segmentGraph::splitChildFromParentInternal( const segId childID )
{
    segment* child = mCache->SegmentStore()->GetSegment( childID );

    if( child->getThreshold() > 1 ){
        return false;
    }

    segment* parent = child->getParent();
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

    segment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();
    child->touchFreshnessForMeshes();

    UpdateSizeListsFromSplit(parentRoot, child);

    return true;
}

void segmentGraph::UpdateSizeListsFromJoin(segment* parent, segment* child)
{
    segment* root = mCache->FindRoot(parent);
    segmentListsLL_->UpdateSizeListsFromJoin(root, child);
}

void segmentGraph::UpdateSizeListsFromSplit(segment* parent, segment* child)
{
    segment* root = mCache->FindRoot(parent);

    const SizeAndNumPieces childInfo =
        computeSegmentSizeWithChildren(child);

    segmentListsLL_->UpdateSizeListsFromSplit(root, child, childInfo);
}

SizeAndNumPieces
segmentGraph::computeSegmentSizeWithChildren(segment* inSeg)
{
    int64_t numVoxels = 0;
    int32_t numPieces = 0;

    segsTempVec_.push_back(inSeg);

    while(!segsTempVec_.empty())
    {
        segment* segRet = segsTempVec_.back();
        segsTempVec_.pop_back();

        FOR_EACH(iter, children_->GetChildren(segRet)){
            segsTempVec_.push_back(*iter);
        }

        const int64_t segSize = segRet->size();

        assert(segSize >= 0);

        numVoxels += segSize;

        assert(numVoxels >= 0);

        ++numPieces;
    }

    SizeAndNumPieces ret = { numVoxels, numPieces };
    return ret;
}
