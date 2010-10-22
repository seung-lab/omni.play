#include "segment/lowLevel/omPagingPtrStore.h"
#include "common/omDebug.h"
#include "datalayer/omMST.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentLists.hpp"
#include "system/cache/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation * segmentation)
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
	if( 0 == value ){
		return NULL;
	}

	OmSegment* seg = mSegments->AddSegment(value);
	seg->RandomizeColor();

	++mNumSegs;
	if (mMaxValue < value) {
		mMaxValue = value;
	}

	return seg;;
}

OmSegment* OmSegmentCacheImpl::GetOrAddSegment(const OmSegID val)
{
	if( 0 == val ){
		return NULL;
	}

	OmSegment* seg = GetSegmentFromValue(val);
	if(NULL == seg){
		seg = AddSegment(val);
	}

	return seg;
}

OmSegmentEdge OmSegmentCacheImpl::findClosestCommonEdge(OmSegment * seg1, OmSegment * seg2)
{
	if( findRoot(seg1) != findRoot(seg2) ){
		//debug(dend, "can't split disconnected objects.\n");
		return OmSegmentEdge();
	}
	if( seg1 == seg2 ){
		//debug(dend, "can't split object from self.\n");
		return OmSegmentEdge();
	}

	OmSegment * s1 = seg1;
	while (0 != s1->getParentSegID()) {
		if(s1->getParentSegID() == seg2->value()) {
			//debug(split, "splitting child from a direct parent\n");
			return OmSegmentEdge(s1);
		}
		s1 = GetSegmentFromValue(s1->getParentSegID());
	}

	OmSegment * s2 = seg2;
	while (0 != s2->getParentSegID()) {
		if(s2->getParentSegID() == seg1->value()) {
			//debug(split, "splitting child from a direct parent\n");
			return OmSegmentEdge(s2);
		}
		s2 = GetSegmentFromValue(s2->getParentSegID());
	}

	OmSegment * nearestCommonPred = 0;

	OmSegment * one;
	OmSegment * two;

	for (quint32 oneID = seg1->value(), twoID; oneID != 0; oneID = one->getParentSegID()) {
		one = GetSegmentFromValue(oneID);
		for (twoID = seg2->value(); twoID != 0 && oneID != twoID; twoID = two->getParentSegID()) {
			two = GetSegmentFromValue(twoID);
		}
		if (oneID == twoID) {
			nearestCommonPred = one;
			break;
		}
	}

	assert(nearestCommonPred != 0);

	double minThresh = 100.0;
	OmSegment * minChild = 0;
	for (one = seg1; one != nearestCommonPred; one = GetSegmentFromValue(one->getParentSegID())) {
		if (one->getThreshold() < minThresh) {
			minThresh = one->getThreshold();
			minChild = one;
		}
	}

	for (one = seg2; one != nearestCommonPred; one = GetSegmentFromValue(one->getParentSegID())) {
		if (one->getThreshold() < minThresh) {
			minThresh = one->getThreshold();
			minChild = one;
		}
	}

	assert(minChild != 0);
	return OmSegmentEdge(minChild);

}

OmSegmentEdge OmSegmentCacheImpl::SplitEdgeUserAction( OmSegmentEdge e )
{
	return splitChildFromParent( GetSegmentFromValue( e.childID ));
}

OmSegmentEdge OmSegmentCacheImpl::splitChildFromParent( OmSegment * child )
{
	assert( child->getParentSegID() );

	OmSegment * parent = GetSegmentFromValue( child->getParentSegID() );

	if( child->IsValid() == parent->IsValid() &&
	    1 == child->IsValid() ){
		printf("could not split %d from %d\n", child->value(), parent->value());
		return OmSegmentEdge();
	}

	OmSegmentEdge edgeThatGotBroken( parent, child, child->getThreshold() );

	parent->removeChild(child);
	mSegmentGraph.graph_cut(child->value());
	child->setParentSegID(0);

	child->setThreshold(0);

	findRoot(parent)->touchFreshnessForMeshes();
	child->touchFreshnessForMeshes();

	if( isSegmentSelected( parent->value() ) ){
		doSelectedSetInsert( child->value(), true );
	} else {
		doSelectedSetRemove( child->value() );
	}

	if( -1 != child->getEdgeNumber() ){
		const int e = child->getEdgeNumber();
		OmMSTEdge* edges = mSegmentation->getMST()->Edges();

		edges[e].userSplit = 1;
		edges[e].wasJoined = 0;
		edges[e].userJoin  = 0;
		child->setEdgeNumber(-1);
	}

	if( child->getCustomMergeEdge().isValid() ){
		const int numRemoved =
			mManualUserMergeEdgeList.removeAll( child->getCustomMergeEdge() );
		printf("number of user edges removed: %d\n", numRemoved);
		child->setCustomMergeEdge(OmSegmentEdge());
	}

	mSegmentGraph.updateSizeListsFromSplit( parent, child );

	clearCaches();

	return edgeThatGotBroken;
}

std::pair<bool, OmSegmentEdge> OmSegmentCacheImpl::JoinFromUserAction( OmSegmentEdge e )
{
	std::pair<bool, OmSegmentEdge> edge = JoinEdgeFromUser( e );
	if(edge.first){
		mManualUserMergeEdgeList.push_back( edge.second );
	}
	return edge;
}

std::pair<bool, OmSegmentEdge> OmSegmentCacheImpl::JoinEdgeFromUser( OmSegmentEdge e )
{
	const OmSegID childRootID = mSegmentGraph.graph_getRootID(e.childID);
	OmSegment* childRoot = GetSegmentFromValue(childRootID);
	OmSegment* parent = GetSegmentFromValue( e.parentID );
	OmSegment* parentRoot = findRoot(parent);

	if( childRoot == parentRoot ){
		printf("cycle found in user manual edge; skipping edge %d, %d, %f\n",
		       e.childID, e.parentID, e.threshold);
		return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
	}

	if( childRoot->IsValid() != parent->IsValid() ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->value(), parent->value(), childRoot->IsValid(), parent->IsValid() );
		return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
	}

/*
	if( childRoot->IsValid() &&
		childRoot != parentRoot ){
		printf("can't join two validated segments\n");
		return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
	}
*/

	mSegmentGraph.graph_join(childRootID, e.parentID);

	parent->addChild(childRoot);
	childRoot->setParent(parent, e.threshold);
	childRoot->setCustomMergeEdge(e);

	findRoot(parent)->touchFreshnessForMeshes();

	if( isSegmentSelected( e.childID ) ){
		doSelectedSetInsert( parent->value(), true );
	}
	doSelectedSetRemove( e.childID );

	mSegmentGraph.updateSizeListsFromJoin( parent, childRoot );

	return std::pair<bool, OmSegmentEdge>(true,
										  OmSegmentEdge( parent,
														 childRoot,
														 e.threshold ));
}

std::pair<bool, OmSegmentEdge> OmSegmentCacheImpl::JoinFromUserAction( const OmSegID parentID, const OmSegID childUnknownDepthID )
{
	const double threshold = 2.0f;
	return JoinFromUserAction( OmSegmentEdge( parentID, childUnknownDepthID, threshold) );
}

void OmSegmentCacheImpl::JoinTheseSegments( const OmSegIDsSet & segmentList)
{
	if(segmentList.size() < 2 ){
		return;
	}

	OmSegIDsSet set = segmentList; // Join() could modify list

	// The first Segment Id is the parent we join to
	OmSegIDsSet::const_iterator iter = set.begin();
	const OmSegID parentID = *iter;
	++iter;

	// We then iterate through the Segment Ids and join
	// each one to the parent
	while (iter != set.end()) {
		std::pair<bool, OmSegmentEdge> edge =
			JoinFromUserAction( parentID, *iter );
		if(!edge.first){
			printf("WARNING: could not join edge; was a segment validated?\n");
		}
		++iter;
	}

	clearCaches();
}

void OmSegmentCacheImpl::UnJoinTheseSegments( const OmSegIDsSet & segmentList)
{
	if(segmentList.size() < 2 ){
		return;
	}

	OmSegIDsSet set = segmentList; // split() could modify list

	// The first Segment Id is the parent we split from
	OmSegIDsSet::const_iterator iter = set.begin();
	++iter;

	// We then iterate through the Segment Ids and split
	// each one from the parent
	while (iter != set.end()) {
		splitChildFromParent( GetSegmentFromValue(*iter) );
		++iter;
	}

	clearCaches();
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	OmSegment* seg = findRoot( segUnknownDepth );

	switch(seg->GetListType()){
	case om::VALID:
		return getSegmentLists()->Valid().getSegmentSize(seg);
	case om::WORKING:
		return getSegmentLists()->Working().getSegmentSize(seg);
	case om::UNCERTAIN:
		return getSegmentLists()->Uncertain().getSegmentSize(seg);
	default:
		throw OmArgException("unknown type");
	}
}

void OmSegmentCacheImpl::rerootSegmentLists()
{
	rerootSegmentList( mEnabledSet );
	rerootSegmentList( mSelectedSet );
}

void OmSegmentCacheImpl::rerootSegmentList( OmSegIDsSet & set )
{
	OmSegIDsSet old = set;
	set.clear();

	OmSegID rootSegID;
	foreach( const OmSegID & id, old ){
		rootSegID = findRoot( GetSegmentFromValue( id) )->value();
		set.insert( rootSegID );
	}
}

void OmSegmentCacheImpl::refreshTree()
{
	if( mSegmentGraph.graph_doesGraphNeedToBeRefreshed(mMaxValue) ){
		mSegmentGraph.initialize(this);
		foreach( const OmSegmentEdge & e, mManualUserMergeEdgeList ){
			JoinEdgeFromUser(e);
		}
		setGlobalThreshold();
	} else {
		resetGlobalThreshold();
	}
}

void OmSegmentCacheImpl::setGlobalThreshold()
{
	boost::shared_ptr<OmMST> mst = mSegmentation->getMST();

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
	boost::shared_ptr<OmMST> mst = mSegmentation->getMST();

	printf("resetting global threshold to %f...\n", mst->UserThreshold());

	mSegmentGraph.resetGlobalThreshold(mst);
	rerootSegmentLists();
	clearCaches();

	printf("done\n");
}

boost::shared_ptr<OmSegmentLists> OmSegmentCacheImpl::getSegmentLists() {
	return getSegmentation()->GetSegmentLists();
}

void OmSegmentCacheImpl::Flush()
{
	mSegments->Flush();
}
