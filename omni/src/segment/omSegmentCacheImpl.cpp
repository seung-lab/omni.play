#include "common/omDebug.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentLists.hpp"
#include "system/cache/omCacheManager.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarantees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation* segmentation)
	: OmSegmentCacheImplLowLevel(segmentation)
{
}

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

	OmSegment* seg = mSegments->AddSegment(value);
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
	mSegmentGraph.graph_cut(child->value());
	child->setParent(NULL); // TODO: also set threshold???

	child->setThreshold(0);

	findRoot(parent)->touchFreshnessForMeshes();
	child->touchFreshnessForMeshes();

	if(isSegmentSelected(parent->value())){
		doSelectedSetInsert(child->value(), true);
	} else {
		doSelectedSetRemove(child->value());
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
		const int numRemoved =
			userEdges()->RemoveEdge(child->getCustomMergeEdge());
		printf("number of user edges removed: %d\n", numRemoved);
		child->setCustomMergeEdge(OmSegmentEdge());
	}

	mSegmentGraph.updateSizeListsFromSplit(parent, child);

	clearCaches();

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
		userEdges()->AddEdge(edge.second);
	}
	return edge;
}

std::pair<bool, OmSegmentEdge>
OmSegmentCacheImpl::JoinEdgeFromUser(const OmSegmentEdge& e)
{
	const OmSegID childRootID = mSegmentGraph.graph_getRootID(e.childID);
	OmSegment* childRoot = GetSegment(childRootID);
	OmSegment* parent = GetSegment(e.parentID);
	OmSegment* parentRoot = findRoot(parent);

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

	mSegmentGraph.graph_join(childRootID, e.parentID);

	parent->addChild(childRoot);
	childRoot->setParent(parent, e.threshold);
	childRoot->setCustomMergeEdge(e);

	findRoot(parent)->touchFreshnessForMeshes();

	if(isSegmentSelected(e.childID)){
		doSelectedSetInsert(parent->value(), true);
	}
	doSelectedSetRemove(e.childID);

	mSegmentGraph.updateSizeListsFromJoin(parent, childRoot);

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

		if(!edge.first){
			printf("WARNING: could not join edge; was a segment validated?\n");
		} else {
			ret.insert(segID);
		}

		++iter;
	}

	clearCaches();

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

	clearCaches();

	if(!ret.empty()){
		ret.insert(parentID);
	}

	return ret;
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren(OmSegment * segUnknownDepth)
{
	OmSegment* seg = findRoot(segUnknownDepth);
	return getSegmentLists()->getSegmentSize(seg);
}

void OmSegmentCacheImpl::rerootSegmentLists()
{
	rerootSegmentList(mEnabledSet);
	rerootSegmentList(mSelectedSet);
}

void OmSegmentCacheImpl::rerootSegmentList(OmSegIDsSet& set)
{
	OmSegIDsSet old = set;
	set.clear();

	OmSegID rootSegID;
	foreach(const OmSegID& id, old){
		rootSegID = findRoot(GetSegment(id))->value();
		set.insert(rootSegID);
	}
}

void OmSegmentCacheImpl::refreshTree()
{
	if(mSegmentGraph.graph_doesGraphNeedToBeRefreshed(mMaxValue)){
		mSegmentGraph.initialize(this);
		foreach(const OmSegmentEdge& e, userEdges()->Edges()){
			JoinEdgeFromUser(e);
		}
		setGlobalThreshold();
	} else {
		resetGlobalThreshold();
	}
}

void OmSegmentCacheImpl::setGlobalThreshold()
{
	OmMST* mst = segmentation_->MST();

	if(!mst->isValid()){
		printf("no graph found...\n");
		return;
	}

	printf("setting global threshold to %f...\n", mst->UserThreshold());
	mSegmentGraph.setGlobalThreshold(mst);

	mSelectedSet.clear();
	clearCaches();

	printf("done\n");
}

void OmSegmentCacheImpl::resetGlobalThreshold()
{
	OmMST* mst = segmentation_->MST();

	printf("resetting global threshold to %f...\n", mst->UserThreshold());

	mSegmentGraph.resetGlobalThreshold(mst);
	rerootSegmentLists();
	clearCaches();

	printf("done\n");
}

OmSegmentLists* OmSegmentCacheImpl::getSegmentLists() {
	return GetSegmentation()->SegmentLists();
}

void OmSegmentCacheImpl::Flush(){
	mSegments->Flush();
}

OmUserEdges* OmSegmentCacheImpl::userEdges(){
	return segmentation_->MSTUserEdges();
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
