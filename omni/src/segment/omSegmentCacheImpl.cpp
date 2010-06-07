#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omCacheManager.h"
#include "system/omProjectData.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataPath.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation * segmentation, OmSegmentCache * cache )
	: mSegmentation(segmentation)
	, mParentCache( cache )
{
	mMaxValue = 0;

	mNumSegs = 0;
	mNumTopLevelSegs = 0;

        mAllSelected = false;
        mAllEnabled = false;

	amInBatchMode = false;
	needToFlush = false;
	mPageSize = 10000;
	mAllPagesLoaded = false;

	mGraph = NULL;
}

OmSegmentCacheImpl::~OmSegmentCacheImpl()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		
		for( quint32 i = 0; i < mPageSize; ++i ){
			delete mValueToSegPtrHash[pageNum][i];
		}
	}
	
	delete mGraph;
}

OmSegID OmSegmentCacheImpl::getSegmentationID()
{
	return mSegmentation->GetId();
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

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains( pageNum ) ) {
		mValueToSegPtrHash[ pageNum ].resize(mPageSize, NULL);
		validPageNumbers.insert( pageNum );
		loadedPageNumbers.insert( pageNum );
	}

	OmSegment * seg = new OmSegment( value, mParentCache);
	mValueToSegPtrHash[ pageNum ][value % mPageSize] = seg;

	++mNumSegs;
	++mNumTopLevelSegs;

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

bool OmSegmentCacheImpl::isValueAlreadyMappedToSegment( const OmSegID value )
{
	if (0 == value) {
		return false;
	}

	const PageNum pageNum = getValuePageNum(value);

	if( !validPageNumbers.contains( pageNum ) ){
		return false;
	}

	if( !loadedPageNumbers.contains( pageNum ) ){
		LoadValuePage( pageNum );
	}

	if( NULL != mValueToSegPtrHash[pageNum][value % mPageSize]){
		return true;
	}

	return false;
}

OmSegID OmSegmentCacheImpl::getNextValue()
{
	++mMaxValue;
	return mMaxValue;
}

OmSegID OmSegmentCacheImpl::GetNumSegments()
{
	return mNumSegs;
}

OmSegID OmSegmentCacheImpl::GetNumTopSegments()
{
	loadTreeIfNeeded();
	return mNumTopLevelSegs;
}

quint32 OmSegmentCacheImpl::numberOfSelectedSegments()
{
	return mSelectedSet.size();
}

bool OmSegmentCacheImpl::AreSegmentsSelected()
{
	if( 0 == numberOfSelectedSegments() ){
		return false;
	}

	return true;
}

OmSegIDsSet & OmSegmentCacheImpl::GetSelectedSegmentIdsRef()
{
	loadTreeIfNeeded();
        return mSelectedSet;
}

OmSegIDsSet & OmSegmentCacheImpl::GetEnabledSegmentIdsRef()
{
	loadTreeIfNeeded();
        return mEnabledSet;
}

void OmSegmentCacheImpl::SetAllEnabled(bool enabled)
{
	mAllEnabled = enabled;
	mEnabledSet.clear();
}

void OmSegmentCacheImpl::SetAllSelected(bool selected)
{
        mAllSelected = selected;
	mSelectedSet.clear();
}

bool OmSegmentCacheImpl::isSegmentEnabled( OmSegID segID )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

	if( mAllEnabled ||
	    mEnabledSet.contains( rootID ) ){
		return true;
	}

	return false;
}

bool OmSegmentCacheImpl::isSegmentSelected( OmSegID segID )
{
	return isSegmentSelected( GetSegmentFromValue( segID ) );
}

bool OmSegmentCacheImpl::isSegmentSelected( OmSegment * seg )
{
	const OmSegID rootID = findRoot( seg )->getValue();

	if( mAllSelected ||
	    mSelectedSet.contains( rootID ) ){
		return true;
	}

	return false;
}

void OmSegmentCacheImpl::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();
	clearCaches();

	if (isEnabled) {
		mEnabledSet.insert( rootID );
	} else {
		mEnabledSet.remove( rootID );
	}
}

void OmSegmentCacheImpl::setSegmentSelected( OmSegID segID, bool isSelected )
{
	setSegmentSelectedBatch( segID, isSelected );
	clearCaches();
}

void OmSegmentCacheImpl::setSegmentSelectedBatch( OmSegID segID, bool isSelected )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

	if (isSelected) {
		mSelectedSet.insert( rootID );
	} else {
		mSelectedSet.remove( rootID );
		assert( !mSelectedSet.contains( segID ));
	}
}

void OmSegmentCacheImpl::setSegmentName( OmSegID segID, QString name )
{
	segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImpl::getSegmentName( OmSegID segID )
{
	if( segmentCustomNames.contains(segID ) ){
		return segmentCustomNames.value(segID);
	}

	return ""; //QString("segment%1").arg(segID);
}

void OmSegmentCacheImpl::setSegmentNote( OmSegID segID, QString note )
{
	segmentNotes[ segID ] = note;
}

QString OmSegmentCacheImpl::getSegmentNote( OmSegID segID )
{
	if( segmentNotes.contains(segID ) ){
		return segmentNotes.value(segID);
	}

	return "";
}

void OmSegmentCacheImpl::addToDirtySegmentList( OmSegment* seg)
{
	if( amInBatchMode ){
		needToFlush = true;
	} else {
		dirtySegmentPages.insert( getValuePageNum( seg->getValue() ) );
	}
}

void OmSegmentCacheImpl::flushDirtySegments()
{
	if( amInBatchMode ){
		if( !needToFlush ){
			return;
		}
		SaveAllLoadedPages();
		needToFlush = false;
	} else {
		SaveDirtySegmentPages();
		dirtySegmentPages.clear();
	}
}

void OmSegmentCacheImpl::splitTwoChildren(OmSegment * seg1, OmSegment * seg2)
{
	if( findRoot(seg1) != findRoot(seg2) ){
		debug("dend", "can't split disconnected objects.\n");
		return;
	}
        if( seg1 == seg2 ){
                debug("dend", "can't split object from self.\n");
                return;
        }

	OmSegment * s1 = seg1;

	while (0 != s1->mParentSegID) {
		if(s1->mParentSegID == seg2->mValue) {
			debug("split", "splitting child from a direct parent\n");
			splitChildFromParent(s1);
			return;
		}
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	} 
	
	OmSegment * s2 = seg2;
	while (0 != s2->mParentSegID) {
		if(s2->mParentSegID == seg1->mValue) {
			debug("split", "splitting child from a direct parent\n");
			splitChildFromParent(s2);
			return;
		}
        	s2 = GetSegmentFromValue(s2->mParentSegID);	
	} 

	OmSegment * one = seg1;
	OmSegment * two = seg2;
	OmSegment * small1 = seg1;
	OmSegment * small2 = seg2;
	float thresh1 = 1.0;
	float thresh2 = 1.0;
	int count = 0;
	s1 = seg1; 
	while (0 != s1->mParentSegID){
		s2 = seg2;
		while (0 != s2->mParentSegID){
			count++;
			//debug("split", "s1 = %u, s2 = %u\n", s1->getValue(), s2->getValue());
			//debug("split", "s1 = %f, s2 = %f\n", s1->getThreshold(), s2->getThreshold());
			if(s1->mParentSegID == s2->mParentSegID) {
				if(s1->getThreshold() < s2->getThreshold()) {
					one = small1;
				} else {
					one = small2;
				}
				goto onedone;
			}

			if(s1->getThreshold() < thresh1) {
				thresh1 = s1->getThreshold();
				small1 = s1;
			}
			if(s2->getThreshold() < thresh2) {
				thresh2 = s2->getThreshold();
				small2 = s2;
			}
			s2 = GetSegmentFromValue(s2->mParentSegID);
		} 
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	}

onedone:

        thresh1 = 1.0;
        thresh2 = 1.0;
        count = 0;

        s1 = seg2;
	while (0 != s1->mParentSegID){
		s2 = seg1;
                while (0 != s2->mParentSegID){
                        count++;
                        //debug("split", "s1 = %u, s2 = %u\n", s1->getValue(), s2->getValue());
                        //debug("split", "s1 = %f, s2 = %f\n", s1->getThreshold(), s2->getThreshold());
                        if(s1->mParentSegID == s2->mParentSegID) {
                                if(s1->getThreshold() < s2->getThreshold()) {
                                        two = small1;
                                } else {
                                        two = small2;
                                }
				goto twodone;
                        }

                        if(s1->getThreshold() < thresh1) {
                                thresh1 = s1->getThreshold();
                                small1 = s1;
                        }
                        if(s2->getThreshold() < thresh2) {
                                thresh2 = s2->getThreshold();
                                small2 = s2;
                        }
                        s2 = GetSegmentFromValue(s2->mParentSegID);
                } 
                s1 = GetSegmentFromValue(s1->mParentSegID);
        } 

twodone:
	assert(one);
	assert(two);
	if(one->mThreshold > two->mThreshold) {
		splitChildFromParent(one);
	} else {
		splitChildFromParent(two);
	}
}

void OmSegmentCacheImpl::splitChildLowestThreshold( OmSegment * segmentUnknownLevel )
{
	OmSegment * root = findRoot(segmentUnknownLevel);

	double minThreshold = 1;
	OmSegment * segToRemove = NULL;
	foreach( const OmSegID & childID, root->segmentsJoinedIntoMe ){
		OmSegment * child = GetSegmentFromValue( childID );
		if( child->getThreshold() < minThreshold){
			minThreshold = child->getThreshold();
			segToRemove = child;
		}
	}
	
	if( NULL == segToRemove ){
		printf("no children to remove\n");
		return;
	}

	splitChildFromParent( segToRemove );

	printf("removed %d from parent\n", segToRemove->getValue() );
}

void OmSegmentCacheImpl::splitChildFromParent( OmSegment * child )
{
	debug("split", "OmSegmentCacheImpl::splitChildFromParent=%u,%f\n",
			child->getValue(), child->mThreshold);

	assert( child->mParentSegID );

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		printf("not splitting child %d from parent %d: child immutability is %d, but parent's is %d\n",
		       child->mValue, parent->mValue, child->mImmutable, parent->mImmutable );
		return;
	}

	debug("split", "\tparent = %u\n", parent->getValue());

	parent->segmentsJoinedIntoMe.erase( child->getValue() );
        mGraph->get( child->mValue )->cut();
	child->mParentSegID = 0;

	//const float oldChildThreshold = child->mThreshold;
	child->mThreshold = 0;

	if( isSegmentSelected( parent->getValue() ) ){
		debug("split", "parent was selected\n");
		mSelectedSet.insert( child->getValue() );
	} else {
		mSelectedSet.remove( child->getValue() );
	}

	/*
	OmSegmentQueueElement parentElement = { child->mValue, oldChildThreshold };
	parent->queue.push(parentElement);

	OmSegmentQueueElement childElement = { parent->mValue, oldChildThreshold };
	child->queue.push(childElement);
	*/

	++mNumTopLevelSegs;

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

	clearCaches();
}

void OmSegmentCacheImpl::SaveAllLoadedPages()
{
	foreach( const PageNum & pageNum, loadedPageNumbers ){
		doSaveSegmentPage( pageNum );
	}
}

void OmSegmentCacheImpl::SaveDirtySegmentPages()
{
	foreach( const PageNum & pageNum, dirtySegmentPages ){
		doSaveSegmentPage( pageNum );
	}
}

void OmSegmentCacheImpl::doSaveSegmentPage( const PageNum pageNum )
{
	const std::vector<OmSegment*> & page = mValueToSegPtrHash[ pageNum ];
	OmDataArchiveSegment::ArchiveWrite( OmDataPaths::getSegmentPagePath( getSegmentationID(), pageNum ),
					    page, mParentCache );
}

void OmSegmentCacheImpl::turnBatchModeOn( const bool batchMode )
{
	amInBatchMode = batchMode;
}

void OmSegmentCacheImpl::LoadValuePage( const PageNum pageNum )
{
	std::vector<OmSegment*> & page = mValueToSegPtrHash[ pageNum ];
	page.resize( mPageSize, NULL );

	OmDataArchiveSegment::ArchiveRead( OmDataPaths::getSegmentPagePath( getSegmentationID(), pageNum ),
					   page,
					   mParentCache);
	
	loadedPageNumbers.insert( pageNum );

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

void OmSegmentCacheImpl::clearCaches()
{
	OmCacheManager::Freshen(true);
}

void OmSegmentCacheImpl::initializeDynamicTree()
{
	delete mGraph;

	// mMaxValue is a valid segment id, so array needs to be 1 bigger
	const int size =  mMaxValue + 1;
	
	mGraph = new DynamicTreeContainer<OmSegID>( size );
}

// TODO: make clear this is only the INTIAL load
void OmSegmentCacheImpl::loadDendrogram()
{
	initializeDynamicTree();
	mNumTopLevelSegs = mNumSegs;

	resetGlobalThreshold( mSegmentation->mDendThreshold );

	foreach( OmSegmentEdge * e, mManualUserMergeEdgeList ){
		Join(e);
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
		rootSegID = findRoot( GetSegmentFromValue( id) )->getValue();
		set.insert( rootSegID );
		//printf("inserting %d\n", rootSegID );
	}
}

void OmSegmentCacheImpl::Join( OmSegmentEdge * e )
{
	DynamicTree<OmSegID> * childRootDT = mGraph->get( e->childID )->findRoot();

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( e->parentID );
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return;
	}

	childRootDT->join( mGraph->get( e->parentID ) );

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, e->threshold);
	childRoot->mCustomMergeEdge = e;

        if( isSegmentSelected( e->childID ) ){
                mSelectedSet.insert( parent->mValue );
        } 
	mSelectedSet.remove( e->childID );

	--mNumTopLevelSegs;
}

OmSegmentEdge * OmSegmentCacheImpl::Join(OmSegment * parent, OmSegment * childUnknownLevel )
{
	return Join( parent->getValue(), childUnknownLevel->getValue() );
}

OmSegmentEdge * OmSegmentCacheImpl::Join( const OmSegID parentID, const OmSegID childUnknownDepthID )
{
	loadTreeIfNeeded();

	const float threshold = 2.0f;

	DynamicTree<OmSegID> * childRootDT = mGraph->get( childUnknownDepthID )->findRoot();

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( parentID );
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return NULL;
	}

	childRootDT->join( mGraph->get( parentID ) );

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, threshold);

        if( isSegmentSelected( childUnknownDepthID ) ){
                mSelectedSet.insert( parent->mValue );
        } 
	mSelectedSet.remove( childUnknownDepthID );

	--mNumTopLevelSegs;

	OmSegmentEdge * edge = new OmSegmentEdge( parentID, childRoot->mValue, threshold);
	mManualUserMergeEdgeList.push_back( edge );
	childRoot->mCustomMergeEdge = edge;

	return edge;
}

OmSegID OmSegmentCacheImpl::findRootID( const OmSegID segID )
{
	return findRoot( GetSegmentFromValue( segID ) )->getValue();
}

OmSegment * OmSegmentCacheImpl::findRoot( OmSegment * segment )
{
	loadTreeIfNeeded();

	if(0 == segment->mParentSegID) {
		return segment;
	}

	DynamicTree<OmSegID> * rootDT  = mGraph->get( segment->mValue )->findRoot();
	
	return GetSegmentFromValue( rootDT->getKey() );
}

void OmSegmentCacheImpl::loadTreeIfNeeded()
{
	if( NULL != mGraph ){
		return;
	}

	loadDendrogram();
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
		Join( parentID, *iter );
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

const OmColor & OmSegmentCacheImpl::GetColorAtThreshold( OmSegment * segment, const float threshold )
{
	//FIXME: this is wrong (purcaro)

	assert(0);

	OmSegment * seg = segment;

	while( 0 != seg->mParentSegID && seg->mThreshold < threshold ){
		seg = GetSegmentFromValue( seg->mParentSegID );
	}

	printf("threshold %f: in %d, out %d\n", threshold, segment->mValue, seg->mValue );

	return seg->mColorInt;
}

/*
void OmSegmentCacheImpl::resetGlobalThreshold( const float stopPoint )
{
	loadTreeIfNeeded();

	quint32 splitCounter = 0;
	quint32 joinCounter = 0;

	OmSegmentQueueElement sqe;
	OmSegment * otherSeg;
	int numRemoved;

	const std::vector<DynamicTree<OmSegID>*> & treeNodeArray = mGraph->getTreeNodeArray();

	OmSegment * seg;
	for( unsigned int i = 1; i < mGraph->getSize(); ++i) {

		if( NULL == treeNodeArray[ i ] ){
			continue;
		}

		seg = GetSegmentFromValue( treeNodeArray[i]->getKey() );
		assert(seg);

		// try merging...
		while( !seg->queue.empty() ){
			
			sqe = seg->queue.top();

			if( sqe.threshold < stopPoint ){
				break;
			}

			Join( i, sqe.segID, sqe.threshold );
			seg->queue.pop();
					
			otherSeg = GetSegmentFromValue( sqe.segID );
			assert( otherSeg );
			assert( !otherSeg->queue.empty() );

			numRemoved = otherSeg->queue.remove( i, sqe.threshold );
			assert( 1 == numRemoved );

			++joinCounter;
		} 
		
		// try splitting...
		if( 0 == seg->mParentSegID ){
			continue;
		}
		
		if( seg->mThreshold < stopPoint ){
			splitChildFromParent( seg );
			++splitCounter;
		}
        }

	rerootSegmentLists();
	clearCaches();	
	printf("\t threshold %f: %d splits, %d joins performed\n", stopPoint, splitCounter, joinCounter );
}
*/

void OmSegmentCacheImpl::UpdateSegmentSelection( const OmSegIDsSet & ids )
{
	mSelectedSet.clear();

	OmSegIDsSet::const_iterator iter;
	for( iter = ids.begin(); iter != ids.end(); ++iter ){
		setSegmentSelectedBatch( *iter, true );
	}

	clearCaches();	
}

// FIXME: this search could become slow.... (purcaro)
OmSegPtrList * OmSegmentCacheImpl::getRootLevelSegIDs( const unsigned int offset, const int numToGet )
{
	loadTreeIfNeeded();

	OmSegPtrList * ret = new OmSegPtrList();

	OmSegment * seg;
	int counter = 0;
	for( quint32 i = offset; i < mMaxValue; ++i ){

		seg = GetSegmentFromValue( i );
		if( NULL == seg || 0 != seg->mParentSegID ){
			continue;
		}
		
		ret->push_back( seg );
		++counter;

		if( counter >= numToGet ){
			break;
		}
	}
	
	return ret;
}

void OmSegmentCacheImpl::resetGlobalThreshold( const float stopPoint )
{
	printf("setting global threshold to %f...\n", stopPoint);

	doResetGlobalThreshold( mSegmentation->mDend->getQuint32Ptr(), 
				mSegmentation->mDendValues->getFloatPtr(), 
				mSegmentation->mEdgeDisabledByUser->getQuint8Ptr(), 
				mSegmentation->mEdgeWasJoined->getQuint8Ptr(), 
				mSegmentation->mEdgeForceJoin->getQuint8Ptr(), 
				mSegmentation->mDendCount, 
				stopPoint);

	mSelectedSet.clear(); // nuke selected set for now...
	//rerootSegmentLists();
	clearCaches();

	printf("done\n");
}

// TODO: store more threshold info in the segment cache, and reduce size of walk...
// NOTE: assuming incoming data is an edge list
void OmSegmentCacheImpl::doResetGlobalThreshold( const quint32 * nodes, 
						 const float * thresholds, 
						 quint8 * edgeDisabledByUser,
						 quint8 * edgeWasJoined,
						 quint8 * edgeForceJoin,
						 const int numEdges, 
						 const float stopThreshold )
{
	printf("\t %d edges...", numEdges);
	fflush(stdout);

	OmSegID childID;
	OmSegID parentID;
	float threshold;

	for(int i = 0; i < numEdges; ++i) {
		if( 1 == edgeDisabledByUser[i] ){
			continue;
		}

		childID = nodes[i];
		threshold = thresholds[i];
		
		if( threshold >= stopThreshold ||
		    1 == edgeForceJoin[i] ){ // join
			if( 1 == edgeWasJoined[i] ){
				continue;
			}
			parentID = nodes[i + numEdges ];
			if( JoinInternal( parentID, childID, threshold, i) ){
				edgeWasJoined[i] = 1;
			} else {
				edgeDisabledByUser[i] = 1;
			}
		} else { // split
			if( 0 == edgeWasJoined[i] ){
				continue;
			}
			if( splitChildFromParentInternal( childID ) ){
				edgeWasJoined[i] = 0;
			} else {
				edgeForceJoin[i] = 1;
			}
		}
        }

	printf("done\n");
}

bool OmSegmentCacheImpl::JoinInternal( const OmSegID parentID, 
				       const OmSegID childUnknownDepthID, 
				       const float threshold,
				       const int edgeNumber )
{
	DynamicTree<OmSegID> * childRootDT = mGraph->get( childUnknownDepthID )->findRoot();

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( parentID );

	if( childRoot == findRoot( parent ) ){
		return false;
	}
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return false;
	}

	childRootDT->join( mGraph->get( parentID ) );

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, threshold);
	childRoot->mEdgeNumber = edgeNumber;

	--mNumTopLevelSegs;
	
	return true;
}

bool OmSegmentCacheImpl::splitChildFromParentInternal( const OmSegID childID )
{
	OmSegment * child = GetSegmentFromValue( childID );

	assert( child->mParentSegID );

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		printf("not splitting child %d from parent %d: child immutability is %d, but parent's is %d\n",
		       child->mValue, parent->mValue, child->mImmutable, parent->mImmutable );
		return false;
	}
	
	if( child->mThreshold > 1 ){
		return false;
	}
	
	parent->segmentsJoinedIntoMe.erase( child->mValue );
        mGraph->get( child->mValue )->cut();
	child->mParentSegID = 0;
	child->mEdgeNumber = -1;

	++mNumTopLevelSegs;

	return true;
}

void OmSegmentCacheImpl::setAsValidated( const OmSegIDsList & segs )
{
	quint8 * edgeForceJoin = mSegmentation->mEdgeForceJoin->getQuint8Ptr();

	OmSegment * seg;

	OmSegIDsList::const_iterator iter;
	for( iter=segs.begin(); iter != segs.end(); ++iter ){
		seg = GetSegmentFromValue( *iter );

		if( -1 == seg->mEdgeNumber ){
			continue;
		}

		edgeForceJoin[ seg->mEdgeNumber ] = 1;
	}
}

void OmSegmentCacheImpl::unsetAsValidated( const OmSegIDsList & segs )
{
	quint8 * edgeForceJoin = mSegmentation->mEdgeForceJoin->getQuint8Ptr();

	OmSegment * seg;

	OmSegIDsList::const_iterator iter;
	for( iter=segs.begin(); iter != segs.end(); ++iter ){
		seg = GetSegmentFromValue( *iter );

		if( -1 == seg->mEdgeNumber ){
			continue;
		}

		edgeForceJoin[ seg->mEdgeNumber ] = 0;
	}
}
