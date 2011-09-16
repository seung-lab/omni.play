#include "common/omDebug.h"
#include "project/omProjectGlobals.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentsImpl.h"
#include "system/cache/omCacheManager.h"
#include "utility/omRandColorFile.hpp"
#include "volume/omSegmentation.h"

// entry into this class via OmSegments hopefully guarantees proper locking...

OmSegmentsImpl::OmSegmentsImpl(OmSegmentation* segmentation,
                               OmSegmentsStore* segmentPages)
    : OmSegmentsImplLowLevel(segmentation, segmentPages)
    , userEdges_(NULL)
{}

OmSegmentsImpl::~OmSegmentsImpl()
{}

OmSegment* OmSegmentsImpl::AddSegment()
{
    const OmSegID newValue = getNextValue();

    assert(newValue);

    OmSegment* newSeg = AddSegment(newValue);
    growGraphIfNeeded(newSeg);
    return newSeg;
}

OmSegment* OmSegmentsImpl::AddSegment(const OmSegID value)
{
    if(0 == value){
        return NULL;
    }

    OmSegment* seg = store_->AddSegment(value);
    seg->SetColor(OmProject::Globals().RandColorFile().GetRandomColor(value));

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

    OmSegment* seg = store_->GetSegment(val);

    if(NULL == seg){
        seg = AddSegment(val);
    }

    return seg;
}

OmSegmentEdge OmSegmentsImpl::SplitEdgeUserAction(const OmSegmentEdge& e)
{
    boost::optional<std::string> splittableTest = IsEdgeSplittable(e);
    if(splittableTest)
    {
        std::cout << "Split error: " << *splittableTest << "\n";
        return OmSegmentEdge();
    }

    OmSegment* child = store_->GetSegment(e.childID);

    return splitChildFromParentNoTest(child);
}

boost::optional<std::string> OmSegmentsImpl::IsEdgeSplittable(const OmSegmentEdge& e)
{
	if(!e.isValid()){
		return std::string("invalid edge");
    }

	OmSegment* child = store_->GetSegment(e.childID);

	return IsSegmentSplittable(child);
}

boost::optional<std::string> OmSegmentsImpl::IsSegmentSplittable(OmSegment* child)
{
	if(!child->getParent()){
		return std::string("segment is the root");
	}

    OmSegment* parent = child->getParent();
    assert(parent);

    if(child->IsValidListType() == parent->IsValidListType() &&
       1 == child->IsValidListType())
    {
        return std::string("segments are valid");
    }

    return boost::optional<std::string>();
}

boost::optional<std::string> OmSegmentsImpl::IsSegmentCuttable(OmSegment* seg)
{
    if(seg->IsValidListType()) {
        return std::string("segment is valid");
    }

    return boost::optional<std::string>();
}

OmSegmentEdge OmSegmentsImpl::splitChildFromParentNoTest(OmSegment* child)
{
    OmSegment* parent = child->getParent();

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

    if(-1 != child->getEdgeNumber())
    {
        const int e = child->getEdgeNumber();
        OmMSTEdge* edges = segmentation_->MST()->Edges();

        edges[e].userSplit = 1;
        edges[e].wasJoined = 0;
        edges[e].userJoin  = 0;
        child->setEdgeNumber(-1);
    }

    if(child->getCustomMergeEdge().isValid())
    {
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
    OmSegment* childRoot = store_->GetSegment(childRootID);
    OmSegment* parent = store_->GetSegment(e.parentID);
    OmSegment* parentRoot = FindRoot(parent);

    if(childRoot == parentRoot)
    {
        printf("cycle found in user manual edge; skipping edge %d, %d, %f\n",
               e.childID, e.parentID, e.threshold);
        return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
    }

    if(childRoot->IsValidListType() != parent->IsValidListType())
    {
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
    while (iter != set.end())
    {
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
    while (iter != set.end())
    {
        const OmSegID segID = *iter;

        OmSegment* child= store_->GetSegment(segID);

        boost::optional<std::string> splittableTest = IsSegmentSplittable(child);

        if(!splittableTest)
        {
            splitChildFromParentNoTest(child);
            ret.insert(segID);

        } else {
            std::cout << "Split error: " << *splittableTest << "\n";
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
    printf("setting size threshold to %f...\n", mst->UserSizeThreshold());

    segmentGraph_.SetGlobalThreshold(mst);
    SegmentSelection().Clear();
}

void OmSegmentsImpl::resetGlobalThreshold(OmMST* mst)
{
    printf("resetting global threshold to %f...\n", mst->UserThreshold());
    printf("resetting size threshold to %f...\n", mst->UserSizeThreshold());

    segmentGraph_.ResetGlobalThreshold(mst);
    rerootSegmentLists();
}

void OmSegmentsImpl::Flush(){
    store_->Flush();
}

bool OmSegmentsImpl::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
{
    FOR_EACH(iter, ids)
    {
        OmSegment* seg = store_->GetSegment(*iter);

        if(!seg){
            continue;
        }

        if(seg->IsValidListType()){
            return true;
        }
    }

    return false;
}

OmSegmentChildren* OmSegmentsImpl::Children()
{
	// TODO: lock?
    return segmentGraph_.Children();
}

std::vector<OmSegmentEdge> OmSegmentsImpl::CutSegment(OmSegment* seg)
{
	std::vector<OmSegmentEdge> edges;

	if(seg->getParent()){
		edges.push_back(splitChildFromParentNoTest(seg));

	} else
	{
		const segChildCont_t children = segmentGraph_.Children()->GetChildren(seg);

		edges.reserve(children.size());

		FOR_EACH(iter, children){
			edges.push_back(splitChildFromParentNoTest(*iter));
		}
    }

	return edges;
}

bool OmSegmentsImpl::JoinEdges(const std::vector<OmSegmentEdge>& edges)
{
	bool joinedAllEdges = true;

	FOR_EACH(iter, edges)
	{
        std::pair<bool, OmSegmentEdge> edge = JoinEdgeFromUser(*iter);

        if(!edge.first){
            joinedAllEdges = false;
        }
	}

	return joinedAllEdges;
}
