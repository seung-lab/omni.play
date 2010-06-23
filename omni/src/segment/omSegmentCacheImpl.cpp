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

OmSegmentEdge * OmSegmentCacheImpl::splitTwoChildren(OmSegment * seg1, OmSegment * seg2)
{
	if( findRoot(seg1) != findRoot(seg2) ){
		debug("dend", "can't split disconnected objects.\n");
		return NULL;
	}
        if( seg1 == seg2 ){
                debug("dend", "can't split object from self.\n");
                return NULL;
        }

	OmSegment * s1 = seg1;
	while (0 != s1->mParentSegID) {
		if(s1->mParentSegID == seg2->mValue) {
			debug("split", "splitting child from a direct parent\n");
			return splitChildFromParent(s1);
		}
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	} 
	
	OmSegment * s2 = seg2;
	while (0 != s2->mParentSegID) {
		if(s2->mParentSegID == seg1->mValue) {
			debug("split", "splitting child from a direct parent\n");
			return splitChildFromParent(s2);
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
        return splitChildFromParent(minChild);
}

OmSegmentEdge * OmSegmentCacheImpl::splitChildFromParent( OmSegment * child )
{
	assert( child->mParentSegID );

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		printf("not splitting child %d from parent %d: child immutability is %d and parent's is %d\n",
		       child->mValue, parent->mValue, child->mImmutable, parent->mImmutable );
		return NULL;
	}

	OmSegmentEdge * edgeThatGotBroken = new OmSegmentEdge( parent, child, child->mThreshold );

	parent->segmentsJoinedIntoMe.erase( child->getValue() );
        mSegmentGraph.cut(child->mValue);
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

	if( NULL != child->mCustomMergeEdge ){
		mManualUserMergeEdgeList.removeAll( child->mCustomMergeEdge );
		delete child->mCustomMergeEdge;
		child->mCustomMergeEdge = NULL;
	}

	updateSizeListsFromSplit( parent, child );

	clearCaches();

	return edgeThatGotBroken;
}

OmSegmentEdge * OmSegmentCacheImpl::JoinFromUserAction( OmSegmentEdge * e )
{
	OmSegmentEdge * edge = JoinEdge( e );
	mManualUserMergeEdgeList.push_back( edge );
	return edge;
}

OmSegmentEdge * OmSegmentCacheImpl::JoinEdge( OmSegmentEdge * e )
{
	const OmSegID childRootID = mSegmentGraph.getRootID(e->childID);
	OmSegment * childRoot = GetSegmentFromValue(childRootID);
	OmSegment * parent = GetSegmentFromValue( e->parentID );
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return NULL;
	}

	mSegmentGraph.join(childRootID, e->parentID);

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, e->threshold);
	childRoot->mCustomMergeEdge = e;

        if( isSegmentSelected( e->childID ) ){
                doSelectedSetInsert( parent->mValue );
        } 
	doSelectedSetRemove( e->childID );

	updateSizeListsFromJoin( parent, childRoot );

	return new OmSegmentEdge( parent, childRoot, e->threshold );
}

OmSegmentEdge * OmSegmentCacheImpl::JoinFromUserAction( const OmSegID parentID, const OmSegID childUnknownDepthID )
{
	const float threshold = 2.0f;
	return JoinFromUserAction( new OmSegmentEdge( parentID, childUnknownDepthID, threshold) );
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
		ids = mValidListBySize.getAPageWorthOfSegmentIDs(offset, numToGet, startSeg);
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
                return mValidListBySize.size();
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
		OmSegmentListBySize::swapSegment(seg, mSegmentGraph.mRootListBySize, mValidListBySize);
	} else {
		OmSegmentListBySize::swapSegment(seg, mValidListBySize, mSegmentGraph.mRootListBySize);
	}

        if( -1 == seg->mEdgeNumber ){
        	return;
	}

	quint8 * edgeForceJoin = mSegmentation->mEdgeForceJoin->getQuint8Ptr();
	edgeForceJoin[ seg->mEdgeNumber ] = valid;
}

void OmSegmentCacheImpl::refreshTree()
{
	if( mSegmentGraph.doesGraphNeedToBeRefreshed(mMaxValue) ){
		loadDendrogram();
	}
	
	resetGlobalThreshold( mSegmentation->mDendThreshold );
}

void OmSegmentCacheImpl::loadDendrogram()
{
	initialize();

	buildSegmentSizeLists();

	foreach( OmSegmentEdge * e, mManualUserMergeEdgeList ){
		JoinEdge(e);
	}
}

void OmSegmentCacheImpl::setSegmentSelectedBatch( OmSegID segID, bool isSelected )
{
       const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

       if (isSelected) {
               doSelectedSetInsert( rootID );
       } else {
               doSelectedSetRemove( rootID );
               assert( !mSelectedSet.contains( segID ));
       }
}

void OmSegmentCacheImpl::updateSizeListsFromSplit( OmSegment * parent, OmSegment * child )
{
	OmSegment * root = findRoot(parent);
	quint64 newChildSize = computeSegmentSizeWithChildren( child->mValue );
	mSegmentGraph.mRootListBySize.updateFromSplit( root, child, newChildSize );
}

quint64 OmSegmentCacheImpl::computeSegmentSizeWithChildren( const OmSegID segID )
{
	quint64 size = 0;
	OmSegmentIteratorLowLevel iter(this);
	iter.iterOverSegmentID( segID );
	for(OmSegment * seg = iter.getNextSegment(); NULL != seg; seg = iter.getNextSegment()){
		size += seg->mSize;
	}
	return size;
}

void OmSegmentCacheImpl::updateSizeListsFromJoin( OmSegment * parent, OmSegment * child )
{
	OmSegment * root = findRoot(parent);
	mSegmentGraph.mRootListBySize.updateFromJoin( root, child );
	mValidListBySize.updateFromJoin( root, child );
}

void OmSegmentCacheImpl::doSelectedSetInsert( const OmSegID segID)
{
	mSelectedSet.insert( segID );
	addToRecentMap(segID);
}
 		
void OmSegmentCacheImpl::doSelectedSetRemove( const OmSegID segID)
{
	mSelectedSet.remove( segID );
	addToRecentMap(segID);
}
	
void OmSegmentCacheImpl::addToRecentMap( const OmSegID segID )
{
	mRecentRootActivityMap.touch( segID );
}

quint64 OmSegmentCacheImpl::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	OmSegment * seg = findRoot( segUnknownDepth );

	if( seg->mImmutable ) {
		return mValidListBySize.getSegmentSize( seg );
	} 

	return mSegmentGraph.mRootListBySize.getSegmentSize( seg );
}
