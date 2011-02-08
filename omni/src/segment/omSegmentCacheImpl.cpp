#include "common/omDebug.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentLists.hpp"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarantees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation* segmentation)
    : OmSegmentCacheImplLowLevel(segmentation)
    , userEdges_(NULL)
{}

OmSegmentCacheImpl::~OmSegmentCacheImpl()
{
}

OmSegment* OmSegmentCacheImpl::AddSegment()
{
    OmSegment * newSeg = AddSegment(getNextValue());
    growGraphIfNeeded(newSeg);
    return newSeg;
}

OmSegment* OmSegmentCacheImpl::AddSegment(const OmSegID value)
{
    if(0 == value){
        return NULL;
    }

    OmSegment* seg = segmentPages_->AddSegment(value);
    seg->RandomizeColor();

    ++mNumSegs;
    if (mMaxValue < value) {
        mMaxValue = value;
    }

    return seg;
}

OmSegment* OmSegmentCacheImpl::GetOrAddSegment(const OmSegID val)
{
    if(0 == val){
        return NULL;
    }

    OmSegment* seg = GetSegment(val);
    if(NULL == seg){
        seg = AddSegment(val);
    }

    return seg;
}

OmSegmentEdge OmSegmentCacheImpl::SplitEdgeUserAction(const OmSegmentEdge& e)
{
    if(!e.isValid()){
        return OmSegmentEdge();
    }
    return splitChildFromParent(GetSegment(e.childID));
}

OmSegmentEdge OmSegmentCacheImpl::splitChildFromParent(OmSegment * child)
{
    OmSegment* parent = child->getParent();
    assert(parent);

    if(child->IsValidListType() == parent->IsValidListType() &&
       1 == child->IsValidListType()){
        printf("could not split %d from %d (one or more was valid!)\n", child->value(), parent->value());
        return OmSegmentEdge();
    }

    OmSegmentEdge edgeThatGotBroken(parent->value(),
                                    child->value(),
                                    child->getThreshold());

    parent->removeChild(child);
    mSegmentGraph.Cut(child->value());
    child->setParent(NULL); // TODO: also set threshold???

    child->setThreshold(0);

    FindRoot(parent)->touchFreshnessForMeshes();
    child->touchFreshnessForMeshes();

    if(SegmentSelection().isSegmentSelected(parent->value())){
        SegmentSelection().doSelectedSetInsert(child->value(), true);
    } else {
        SegmentSelection().doSelectedSetRemove(child->value());
    }

    if(-1 != child->getEdgeNumber()){
        const int e = child->getEdgeNumber();
        OmMSTEdge* edges = segmentation_->MST()->Edges();

        edges[e].userSplit = 1;
        edges[e].wasJoined = 0;
        edges[e].userJoin  = 0;
        child->setEdgeNumber(-1);
    }

    if(child->getCustomMergeEdge().isValid()){
        userEdges_->RemoveEdge(child->getCustomMergeEdge());
        child->setCustomMergeEdge(OmSegmentEdge());
    }

    mSegmentGraph.UpdateSizeListsFromSplit(parent, child);

    touchFreshness();

    return edgeThatGotBroken;
}

std::pair<bool, OmSegmentEdge>
OmSegmentCacheImpl::JoinFromUserAction(const OmSegmentEdge& e)
{
    if(!e.isValid()){
        return std::make_pair(false, OmSegmentEdge());
    }

    std::pair<bool, OmSegmentEdge> edge = JoinEdgeFromUser(e);
    if(edge.first){
        userEdges_->AddEdge(edge.second);
    }
    return edge;
}

std::pair<bool, OmSegmentEdge>
OmSegmentCacheImpl::JoinEdgeFromUser(const OmSegmentEdge& e)
{
    const OmSegID childRootID = mSegmentGraph.Root(e.childID);
    OmSegment* childRoot = GetSegment(childRootID);
    OmSegment* parent = GetSegment(e.parentID);
    OmSegment* parentRoot = FindRoot(parent);

    if(childRoot == parentRoot){
        printf("cycle found in user manual edge; skipping edge %d, %d, %f\n",
               e.childID, e.parentID, e.threshold);
        return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
    }

    if(childRoot->IsValidListType() != parent->IsValidListType()){
        printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
               childRoot->value(), parent->value(), childRoot->IsValidListType(), parent->IsValidListType());
        return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
    }

/*
  if(childRoot->IsValidListType() &&
  childRoot != parentRoot){
  printf("can't join two validated segments\n");
  return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
  }
*/

    mSegmentGraph.Join(childRootID, e.parentID);

    parent->addChild(childRoot);
    childRoot->setParent(parent, e.threshold);
    childRoot->setCustomMergeEdge(e);

    FindRoot(parent)->touchFreshnessForMeshes();

    if(SegmentSelection().isSegmentSelected(e.childID)){
        SegmentSelection().doSelectedSetInsert(parent->value(), true);
    }
    SegmentSelection().doSelectedSetRemove(e.childID);

    mSegmentGraph.UpdateSizeListsFromJoin(parent, childRoot);

    return std::pair<bool, OmSegmentEdge>(true,
                                          OmSegmentEdge(parent->value(),
                                                        childRoot->value(),
                                                        e.threshold));
}

std::pair<bool, OmSegmentEdge>
OmSegmentCacheImpl::JoinFromUserAction(const OmSegID parentID,
                                       const OmSegID childUnknownDepthID)
{
    const double threshold = 2.0f;
    return JoinFromUserAction(OmSegmentEdge(parentID, childUnknownDepthID,
                                            threshold));
}

OmSegIDsSet OmSegmentCacheImpl::JoinTheseSegments(const OmSegIDsSet& segmentList)
{
    if(segmentList.size() < 2){
        return OmSegIDsSet();
    }

    OmSegIDsSet set = segmentList; // Join() could modify list

    OmSegIDsSet ret; // segments actually joined

    // The first Segment Id is the parent we join to
    OmSegIDsSet::const_iterator iter = set.begin();
    const OmSegID parentID = *iter;
    ++iter;

    // We then iterate through the Segment Ids and join
    // each one to the parent
    while (iter != set.end()) {
        const OmSegID segID = *iter;

        std::pair<bool, OmSegmentEdge> edge =
            JoinFromUserAction(parentID, segID);

        if(edge.first){
            ret.insert(segID);
        }

        ++iter;
    }

    touchFreshness();

    if(!ret.empty()){
        ret.insert(parentID);
    }

    return ret;
}

OmSegIDsSet OmSegmentCacheImpl::UnJoinTheseSegments(const OmSegIDsSet& segmentList)
{
    if(segmentList.size() < 2){
        return OmSegIDsSet();
    }

    OmSegIDsSet set = segmentList; // split() could modify list
    OmSegIDsSet ret;

    // The first Segment Id is the parent we split from
    OmSegIDsSet::const_iterator iter = set.begin();
    const OmSegID parentID = *iter;
    ++iter;

    // We then iterate through the Segment Ids and split
    // each one from the parent
    while (iter != set.end()) {
        const OmSegID segID = *iter;
        OmSegmentEdge edge =
            splitChildFromParent(GetSegment(segID));

        if(edge.isValid()){
            printf("WARNING: could not split edge; was a segment validated?\n");
        } else {
            ret.insert(segID);
        }

        ++iter;
    }

    touchFreshness();

    if(!ret.empty()){
        ret.insert(parentID);
    }

    return ret;
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren(OmSegment * segUnknownDepth)
{
    OmSegment* seg = FindRoot(segUnknownDepth);
    return segmentation_->SegmentLists()->getSegmentSize(seg);
}

void OmSegmentCacheImpl::rerootSegmentLists()
{
    EnabledSegments().Reroot();
    SegmentSelection().rerootSegmentList();
}

void OmSegmentCacheImpl::refreshTree()
{
    if(mSegmentGraph.DoesGraphNeedToBeRefreshed(mMaxValue)){
        mSegmentGraph.Initialize(segmentation_, this);

        userEdges_ = segmentation_->MSTUserEdges();
        FOR_EACH(iter, userEdges_->Edges()){
            JoinEdgeFromUser(*iter);
        }

        setGlobalThreshold();

    } else {
        resetGlobalThreshold();
    }
}

void OmSegmentCacheImpl::setGlobalThreshold()
{
    OmMST* mst = segmentation_->MST();

    if(!mst->IsValid()){
        printf("no graph found...\n");
        return;
    }

    printf("setting global threshold to %f...\n", mst->UserThreshold());

    mSegmentGraph.SetGlobalThreshold(mst);
    SegmentSelection().Clear();
    touchFreshness();

    printf("done\n");
}

void OmSegmentCacheImpl::resetGlobalThreshold()
{
    OmMST* mst = segmentation_->MST();

    printf("resetting global threshold to %f...\n", mst->UserThreshold());

    mSegmentGraph.ResetGlobalThreshold(mst);
    rerootSegmentLists();
    touchFreshness();

    printf("done\n");
}

void OmSegmentCacheImpl::Flush(){
    segmentPages_->Flush();
}

bool OmSegmentCacheImpl::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
{
    FOR_EACH(iter, ids){
        OmSegment* seg = GetSegment(*iter);
        if(!seg){
            continue;
        }
        if(seg->IsValidListType()){
            return true;
        }
    }
    return false;
}
