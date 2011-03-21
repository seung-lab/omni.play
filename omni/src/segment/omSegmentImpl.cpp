#include "segment/lowLevel/omSegmentChildren.hpp"
#include "common/omDebug.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegmentsImpl.h"
#include "segment/omSegmentEdge.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegments hopefully guarantees proper locking...

OmSegmentsImpl::OmSegmentsImpl(OmSegmentation* segmentation)
    : OmSegmentsImplLowLevel(segmentation)
    , userEdges_(NULL)
{}

OmSegmentsImpl::~OmSegmentsImpl()
{
}

OmSegment* OmSegmentsImpl::AddSegment()
{
    OmSegment* newSeg = AddSegment(getNextValue());
    growGraphIfNeeded(newSeg);
    return newSeg;
}

OmSegment* OmSegmentsImpl::AddSegment(const OmSegID value)
{
    if(0 == value){
        return NULL;
    }

    OmSegment* seg = segmentPages_->AddSegment(value);
    seg->RandomizeColor();

    ++mNumSegs;
    if(maxValue_.get() < value) {
        maxValue_.set(value);
    }

    return seg;
}

OmSegment* OmSegmentsImpl::GetOrAddSegment(const OmSegID val)
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

OmSegmentEdge OmSegmentsImpl::SplitEdgeUserAction(const OmSegmentEdge& e)
{
    if(!e.isValid()){
        return OmSegmentEdge();
    }
    return splitChildFromParent(GetSegment(e.childID));
}

OmSegmentEdge OmSegmentsImpl::splitChildFromParent(OmSegment * child)
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

    segmentGraph_.Children()->RemoveChild(parent, child);
    segmentGraph_.Cut(child->value());
    child->setParent(NULL);
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

    segmentGraph_.UpdateSizeListsFromSplit(parent, child);

    touchFreshness();

    return edgeThatGotBroken;
}

std::pair<bool, OmSegmentEdge>
OmSegmentsImpl::JoinFromUserAction(const OmSegmentEdge& e)
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
OmSegmentsImpl::JoinEdgeFromUser(const OmSegmentEdge& e)
{
    const OmSegID childRootID = segmentGraph_.Root(e.childID);
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

    segmentGraph_.Join(childRootID, e.parentID);

    segmentGraph_.Children()->AddChild(parent, childRoot);
    childRoot->setParent(parent, e.threshold);
    childRoot->setCustomMergeEdge(e);

    FindRoot(parent)->touchFreshnessForMeshes();

    if(SegmentSelection().isSegmentSelected(e.childID)){
        SegmentSelection().doSelectedSetInsert(parent->value(), true);
    }
    SegmentSelection().doSelectedSetRemove(e.childID);

    segmentGraph_.UpdateSizeListsFromJoin(FindRoot(parent), childRoot);

    return std::pair<bool, OmSegmentEdge>(true,
                                          OmSegmentEdge(parent->value(),
                                                        childRoot->value(),
                                                        e.threshold));
}

std::pair<bool, OmSegmentEdge>
OmSegmentsImpl::JoinFromUserAction(const OmSegID parentID,
                                       const OmSegID childUnknownDepthID)
{
    const double threshold = 2.0f;
    return JoinFromUserAction(OmSegmentEdge(parentID, childUnknownDepthID,
                                            threshold));
}

OmSegIDsSet OmSegmentsImpl::JoinTheseSegments(const OmSegIDsSet& segmentList)
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

OmSegIDsSet OmSegmentsImpl::UnJoinTheseSegments(const OmSegIDsSet& segmentList)
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

void OmSegmentsImpl::rerootSegmentLists()
{
    EnabledSegments().Reroot();
    SegmentSelection().rerootSegmentList();
}

void OmSegmentsImpl::refreshTree()
{
    OmMST* mst = segmentation_->MST();

    if(!mst->IsValid()){
        printf("no graph found...\n");
        return;
    }

    if(segmentGraph_.DoesGraphNeedToBeRefreshed(maxValue_.get()))
    {
        segmentGraph_.Initialize(segmentation_, this);

        setGlobalThreshold(mst);

        userEdges_ = segmentation_->MSTUserEdges();
        FOR_EACH(iter, userEdges_->Edges()){
            JoinEdgeFromUser(*iter);
        }

    } else {
        resetGlobalThreshold(mst);
    }

    RefreshGUIlists();
    touchFreshness();

    printf("done\n");
}

void OmSegmentsImpl::setGlobalThreshold(OmMST* mst)
{
    printf("setting global threshold to %f...\n", mst->UserThreshold());

    segmentGraph_.SetGlobalThreshold(mst);
    SegmentSelection().Clear();
}

void OmSegmentsImpl::resetGlobalThreshold(OmMST* mst)
{
    printf("resetting global threshold to %f...\n", mst->UserThreshold());

    segmentGraph_.ResetGlobalThreshold(mst);
    rerootSegmentLists();
}

void OmSegmentsImpl::Flush(){
    segmentPages_->Flush();
}

bool OmSegmentsImpl::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
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

OmSegmentChildren* OmSegmentsImpl::Children(){
    return segmentGraph_.Children();
}
