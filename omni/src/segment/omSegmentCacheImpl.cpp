#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"

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
       return AddSegment( getNextValue() );
}

OmSegment* OmSegmentCacheImpl::AddSegment( const OmSegID value)
{
	if( 0 == value ){
		return NULL;
	}

	OmSegment * seg = new OmSegment( value, mParentCache);
	mSegments->AddItem( seg );

	++mNumSegs;

	if (mMaxValue < value) {
		mMaxValue = value;
	}

	addToDirtySegmentList(seg);

	return seg;
}

void OmSegmentCacheImpl::AddSegmentsFromChunk(const OmSegIDsSet & data_values, 
					      const OmMipChunkCoord &,
					      boost::unordered_map< OmSegID, unsigned int> * sizes )
{
	OmSegIDsSet::const_iterator iter;
	for( iter = data_values.begin(); iter != data_values.end(); ++iter ){

		if( 0 == *iter ){
			continue;
		}

		OmSegment * seg = GetSegmentFromValue( *iter );

                if( NULL == seg ) {
			seg = AddSegment( *iter );
		}

		assert(seg);

		if( NULL != sizes ){
			seg->mSize += sizes->at(*iter);
		}

		addToDirtySegmentList(seg);
        }
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
	while (0 != s1->mParentSegID) {
		if(s1->mParentSegID == seg2->mValue) {
			debug("split", "splitting child from a direct parent\n");
			return OmSegmentEdge(s1);
		}
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	} 
	
	OmSegment * s2 = seg2;
	while (0 != s2->mParentSegID) {
		if(s2->mParentSegID == seg1->mValue) {
			debug("split", "splitting child from a direct parent\n");
			return OmSegmentEdge(s2);
		}
        	s2 = GetSegmentFromValue(s2->mParentSegID);	
	} 

        OmSegment * nearestCommonPred = 0;
        OmSegment * one;
        OmSegment * two;

        for (quint32 oneID = seg1->mValue, twoID; oneID != 0; oneID = one->mParentSegID) {
          	one = GetSegmentFromValue(oneID);
          	for (twoID = seg2->mValue; twoID != 0 && oneID != twoID; twoID = two->mParentSegID) {
            		two = GetSegmentFromValue(twoID);
          	}
          	if (oneID == twoID) {
              		nearestCommonPred = one;
			break;
          	}
        }

        assert(nearestCommonPred != 0);

        float minThresh = 1.0;
        OmSegment * minChild = 0;
        for (one = seg1; one != nearestCommonPred; one = GetSegmentFromValue(one->mParentSegID)) {
          	if (one->mThreshold < minThresh) {
            		minThresh = one->mThreshold;
            		minChild = one;
          	}
        }

        for (one = seg2; one != nearestCommonPred; one = GetSegmentFromValue(one->mParentSegID)) {
          	if (one->mThreshold < minThresh) {
            		minThresh = one->mThreshold;
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
	assert( child->mParentSegID );

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		return OmSegmentEdge();
	}

	OmSegmentEdge edgeThatGotBroken( parent, child, child->mThreshold );

	parent->segmentsJoinedIntoMe.erase( child->getValue() );
        mSegmentGraph.graph_cut(child->mValue);
	child->mParentSegID = 0;

	child->mThreshold = 0;

	if( isSegmentSelected( parent->getValue() ) ){
		doSelectedSetInsert( child->getValue() );
	} else {
		doSelectedSetRemove( child->getValue() );
	}

	if( -1 != child->mEdgeNumber ){
		const int e = child->mEdgeNumber;
		quint8 * edgeDisabledByUser = mSegmentation->mEdgeDisabledByUser->getQuint8Ptr();
		quint8 * edgeWasJoined = mSegmentation->mEdgeWasJoined->getQuint8Ptr();
		quint8 * edgeForceJoin = mSegmentation->mEdgeForceJoin->getQuint8Ptr();

		edgeDisabledByUser[e] = 1;
		edgeWasJoined[e] = 0;
		edgeForceJoin[e] = 0;
		child->mEdgeNumber = -1;
	}	

	if( child->mCustomMergeEdge.isValid() ){
		mManualUserMergeEdgeList.removeAll( child->mCustomMergeEdge );
		child->mCustomMergeEdge.valid = false;
	}

	mSegmentGraph.updateSizeListsFromSplit( parent, child );

	clearCaches();

	return edgeThatGotBroken;
}

OmSegmentEdge OmSegmentCacheImpl::JoinFromUserAction( OmSegmentEdge e )
{
	OmSegmentEdge edge = JoinEdgeFromUser( e );
	mManualUserMergeEdgeList.push_back( edge );
	return edge;
}

OmSegmentEdge OmSegmentCacheImpl::JoinEdgeFromUser( OmSegmentEdge e )
{
	const OmSegID childRootID = mSegmentGraph.graph_getRootID(e.childID);
	OmSegment * childRoot = GetSegmentFromValue(childRootID);
	OmSegment * parent = GetSegmentFromValue( e.parentID );
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return OmSegmentEdge();
	}

	mSegmentGraph.graph_join(childRootID, e.parentID);

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, e.threshold);
	childRoot->mCustomMergeEdge = e;

        if( isSegmentSelected( e.childID ) ){
                doSelectedSetInsert( parent->mValue );
        } 
	doSelectedSetRemove( e.childID );

	mSegmentGraph.updateSizeListsFromJoin( parent, childRoot );

	return OmSegmentEdge( parent, childRoot, e.threshold );
}

OmSegmentEdge OmSegmentCacheImpl::JoinFromUserAction( const OmSegID parentID, const OmSegID childUnknownDepthID )
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
		JoinFromUserAction( parentID, *iter );
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
		ids = mSegmentGraph.mValidListBySize.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	} else if(NOTVALIDROOT == type) {
		ids = mSegmentGraph.mRootListBySize.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
	} else if(RECENTROOT == type) {
		ids = mRecentRootActivityMap.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
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
                return mSegmentGraph.mValidListBySize.size();
        } else if(NOTVALIDROOT == type) {
                return mSegmentGraph.mRootListBySize.size();
        } else if(RECENTROOT == type) {
                return mRecentRootActivityMap.size();
	}

	assert(0 && "shouldn't reach here, type incorrect\n");
	return 0;
}

void OmSegmentCacheImpl::setAsValidated(OmSegment * seg, const bool valid)
{
	if(valid) {
		OmSegmentListBySize::swapSegment(seg, mSegmentGraph.mRootListBySize, mSegmentGraph.mValidListBySize);
	} else {
		OmSegmentListBySize::swapSegment(seg, mSegmentGraph.mValidListBySize, mSegmentGraph.mRootListBySize);
	}

        if( -1 == seg->mEdgeNumber ){
        	return;
	}

	quint8 * edgeForceJoin = mSegmentation->mEdgeForceJoin->getQuint8Ptr();
	edgeForceJoin[ seg->mEdgeNumber ] = valid;
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	OmSegment * seg = findRoot( segUnknownDepth );

	if( seg->mImmutable ) {
		return mSegmentGraph.mValidListBySize.getSegmentSize( seg );
	} 

	return mSegmentGraph.mRootListBySize.getSegmentSize( seg );
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
		rootSegID = findRoot( GetSegmentFromValue( id) )->getValue();
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
	printf("setting global threshold to %f...\n", mSegmentation->mDendThreshold);

	mSegmentGraph.setGlobalThreshold( mSegmentation->mDend->getQuint32Ptr(), 
					  mSegmentation->mDendValues->getFloatPtr(), 
					  mSegmentation->mEdgeDisabledByUser->getQuint8Ptr(), 
					  mSegmentation->mEdgeWasJoined->getQuint8Ptr(), 
					  mSegmentation->mEdgeForceJoin->getQuint8Ptr(), 
					  mSegmentation->mDendCount, 
					  mSegmentation->mDendThreshold);
	
	mSelectedSet.clear();
	clearCaches();

	printf("done\n");
}

void OmSegmentCacheImpl::resetGlobalThreshold()
{
	printf("resetting global threshold to %f...\n", mSegmentation->mDendThreshold);

	mSegmentGraph.resetGlobalThreshold( mSegmentation->mDend->getQuint32Ptr(), 
					    mSegmentation->mDendValues->getFloatPtr(), 
					    mSegmentation->mEdgeDisabledByUser->getQuint8Ptr(), 
					    mSegmentation->mEdgeWasJoined->getQuint8Ptr(), 
					    mSegmentation->mEdgeForceJoin->getQuint8Ptr(), 
					    mSegmentation->mDendCount, 
					    mSegmentation->mDendThreshold);
	
	rerootSegmentLists();
	clearCaches();

	printf("done\n");
}


