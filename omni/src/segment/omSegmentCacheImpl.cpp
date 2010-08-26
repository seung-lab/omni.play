#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentLists.hpp"
#include "system/cache/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl( OmSegmentation * segmentation, OmSegmentCache * cache )
	: OmSegmentCacheImplLowLevel( segmentation, cache )
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

	OmSegment * seg = new OmSegment( value, mParentCache);
	seg->RandomizeColor();

	mSegments->AddItem( seg );
	++mNumSegs;
	if (mMaxValue < value) {
		mMaxValue = value;
	}

	addToDirtySegmentList(seg);

	return seg;
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

	addToDirtySegmentList(seg);
	return seg;
}

OmSegmentEdge OmSegmentCacheImpl::findClosestCommonEdge(OmSegment * seg1, OmSegment * seg2)
{
	if( findRoot(seg1) != findRoot(seg2) ){
		debug("dend", "can't split disconnected objects.\n");
		return OmSegmentEdge();
	}
        if( seg1 == seg2 ){
                debug("dend", "can't split object from self.\n");
                return OmSegmentEdge();
        }

	OmSegment * s1 = seg1;
	while (0 != s1->getParentSegID()) {
		if(s1->getParentSegID() == seg2->value) {
			debug("split", "splitting child from a direct parent\n");
			return OmSegmentEdge(s1);
		}
        	s1 = GetSegmentFromValue(s1->getParentSegID());
	}

	OmSegment * s2 = seg2;
	while (0 != s2->getParentSegID()) {
		if(s2->getParentSegID() == seg1->value) {
			debug("split", "splitting child from a direct parent\n");
			return OmSegmentEdge(s2);
		}
        	s2 = GetSegmentFromValue(s2->getParentSegID());
	}

        OmSegment * nearestCommonPred = 0;

	OmSegment * one;
	OmSegment * two;

	for (quint32 oneID = seg1->value, twoID; oneID != 0; oneID = one->getParentSegID()) {
	  one = GetSegmentFromValue(oneID);
	  for (twoID = seg2->value; twoID != 0 && oneID != twoID; twoID = two->getParentSegID()) {
	    two = GetSegmentFromValue(twoID);
	  }
	  if (oneID == twoID) {
	    nearestCommonPred = one;
	    break;
	  }
	}

	assert(nearestCommonPred != 0);

        float minThresh = 100.0;
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

	if( child->GetImmutable() == parent->GetImmutable() &&
	    1 == child->GetImmutable() ){
		return OmSegmentEdge();
	}

	OmSegmentEdge edgeThatGotBroken( parent, child, child->getThreshold() );

	parent->removeChild(child->value);
        mSegmentGraph.graph_cut(child->value);
	child->setParentSegID(0);

	child->setThreshold(0);

	findRoot(parent)->touchFreshnessForMeshes();
	child->touchFreshnessForMeshes();

	if( isSegmentSelected( parent->value ) ){
		doSelectedSetInsert( child->value, true );
	} else {
		doSelectedSetRemove( child->value );
	}

	if( -1 != child->getEdgeNumber() ){
		const int e = child->getEdgeNumber();
		quint8 * edgeDisabledByUser = mSegmentation->mst.mEdgeDisabledByUser->getPtr<unsigned char>();
		quint8 * edgeWasJoined = mSegmentation->mst.mEdgeWasJoined->getPtr<unsigned char>();
		quint8 * edgeForceJoin = mSegmentation->mst.mEdgeForceJoin->getPtr<unsigned char>();

		edgeDisabledByUser[e] = 1;
		edgeWasJoined[e] = 0;
		edgeForceJoin[e] = 0;
		child->setEdgeNumber(-1);
	}

	if( child->getCustomMergeEdge().isValid() ){
		mManualUserMergeEdgeList.removeAll( child->getCustomMergeEdge() );
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
	OmSegment * childRoot = GetSegmentFromValue(childRootID);
	OmSegment * parent = GetSegmentFromValue( e.parentID );

	if( childRoot == findRoot( parent ) ){
		printf("cycle found in user manual edge; skipping edge %d, %d, %f\n",
		       e.childID, e.parentID, e.threshold);
		return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
	}

	if( childRoot->GetImmutable() != parent->GetImmutable() ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->value, parent->value, childRoot->GetImmutable(), parent->GetImmutable() );
		return std::pair<bool, OmSegmentEdge>(false, OmSegmentEdge());
	}

	mSegmentGraph.graph_join(childRootID, e.parentID);

	parent->addChild(childRoot->value);
	childRoot->setParent(parent, e.threshold);
	childRoot->setCustomMergeEdge(e);

	findRoot(parent)->touchFreshnessForMeshes();

        if( isSegmentSelected( e.childID ) ){
                doSelectedSetInsert( parent->value, true );
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
	const float threshold = 2.0f;
	return JoinFromUserAction( OmSegmentEdge( parentID, childUnknownDepthID, threshold) );
}

void OmSegmentCacheImpl::JoinTheseSegments( const OmSegIDsSet & segmentList)
{
	if( segmentList.size() < 2 ){
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
	if( segmentList.size() < 2 ){
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

OmSegPtrListWithPage * OmSegmentCacheImpl::getRootLevelSegIDs( const unsigned int offset,
							       const int numToGet,
							       const OmSegIDRootType type,
							       const OmSegID startSeg)
{
	OmSegIDsListWithPage * ids;
	if(VALIDROOT == type) {
		ids = getSegmentLists()->mValidListBySize.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	} else if(NOTVALIDROOT == type) {
		ids = getSegmentLists()->mRootListBySize.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	} else if(RECENTROOT == type) {
		ids = getSegmentLists()->mRecentRootActivityMap.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	} else {
		assert(0 && "Shouldn't call this function to do non special group code.\n");
	}

	OmSegPtrList retPtrs = OmSegPtrList();

	// TODO: make a little stuct of the data the GUI needs...
	OmSegIDsList::const_iterator iter;
	for( iter = ids->list.begin(); iter != ids->list.end(); ++iter ){
		retPtrs.push_back( GetSegmentFromValue(*iter) );
	}

	OmSegPtrListWithPage * ret = new OmSegPtrListWithPage( retPtrs, ids->mPageOffset );

	delete ids;

	return ret;
}

quint64 OmSegmentCacheImpl::getSegmentListSize(OmSegIDRootType type)
{
        if(VALIDROOT == type) {
                return getSegmentLists()->mValidListBySize.size();
        } else if(NOTVALIDROOT == type) {
                return getSegmentLists()->mRootListBySize.size();
        } else if(RECENTROOT == type) {
                return getSegmentLists()->mRecentRootActivityMap.size();
	}

	assert(0 && "shouldn't reach here, type incorrect\n");
	return 0;
}

void OmSegmentCacheImpl::setAsValidated(OmSegment * seg, const bool valid)
{
	if(valid) {
		getSegmentLists()->moveSegmentFromRootToValid(seg);
	} else {
		getSegmentLists()->moveSegmentFromValidToRoot(seg);
	}

        if( -1 == seg->getEdgeNumber() ){
        	return;
	}

	quint8 * edgeForceJoin = mSegmentation->mst.mEdgeForceJoin->getPtr<unsigned char>();
	edgeForceJoin[ seg->getEdgeNumber() ] = valid;
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	OmSegment * seg = findRoot( segUnknownDepth );

	if( seg->GetImmutable() ) {
		return getSegmentLists()->mValidListBySize.getSegmentSize( seg );
	}

	return getSegmentLists()->mRootListBySize.getSegmentSize( seg );
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
		rootSegID = findRoot( GetSegmentFromValue( id) )->value;
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
  if(!mSegmentation->mst.isValid()){
    printf("no graph found...\n");
    return;
  }

  printf("setting global threshold to %f...\n", mSegmentation->mst.mDendThreshold);
  mSegmentGraph.setGlobalThreshold( mSegmentation->mst.mDend->getPtr<unsigned int>(),
				    mSegmentation->mst.mDendValues->getPtr<float>(),
				    mSegmentation->mst.mEdgeDisabledByUser->getPtr<unsigned char>(),
				    mSegmentation->mst.mEdgeWasJoined->getPtr<unsigned char>(),
				    mSegmentation->mst.mEdgeForceJoin->getPtr<unsigned char>(),
				    mSegmentation->mst.mDendCount,
				    mSegmentation->mst.mDendThreshold);

  mSelectedSet.clear();
  clearCaches();

  printf("done\n");
}

void OmSegmentCacheImpl::resetGlobalThreshold()
{
	printf("resetting global threshold to %f...\n", mSegmentation->mst.mDendThreshold);

	mSegmentGraph.resetGlobalThreshold( mSegmentation->mst.mDend->getPtr<unsigned int>(),
					    mSegmentation->mst.mDendValues->getPtr<float>(),
					    mSegmentation->mst.mEdgeDisabledByUser->getPtr<unsigned char>(),
					    mSegmentation->mst.mEdgeWasJoined->getPtr<unsigned char>(),
					    mSegmentation->mst.mEdgeForceJoin->getPtr<unsigned char>(),
					    mSegmentation->mst.mDendCount,
					    mSegmentation->mst.mDendThreshold);

	rerootSegmentLists();
	clearCaches();

	printf("done\n");
}

boost::shared_ptr<OmSegmentLists> OmSegmentCacheImpl::getSegmentLists() {
	return getSegmentation()->getSegmentLists();
}
