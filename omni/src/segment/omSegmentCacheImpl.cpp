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

	OmSegment * s1;
	OmSegment * s2;

	s1 = seg1; 
	do {
		if(s1->mParentSegID == seg2->mValue) {
			debug("split", "splitting child from a direct parent\n");
			splitChildFromParent(s1);
			return;
		}
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	} while (0 != s1->mParentSegID);

	s2 = seg2; 
	do {
		if(s2->mParentSegID == seg1->mValue) {
			debug("split", "splitting child from a direct parent\n");
			splitChildFromParent(s2);
			return;
		}
        	s2 = GetSegmentFromValue(s2->mParentSegID);	
	} while (0 != s2->mParentSegID);

	OmSegment * one;
	OmSegment * two;
	OmSegment * small1;
	OmSegment * small2;
	float thresh1 = 1.0;
	float thresh2 = 1.0;
	int count = 0;
	s1 = seg1; 
	do {
		s2 = seg2;
		do {
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
		} while (0 != s2->mParentSegID);
        	s1 = GetSegmentFromValue(s1->mParentSegID);	
	} while (0 != s1->mParentSegID);

onedone:

        thresh1 = 1.0;
        thresh2 = 1.0;
        count = 0;

        s1 = seg2;
        do {
		s2 = seg1;
                do {
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
                } while (0 != s2->mParentSegID);
                s1 = GetSegmentFromValue(s1->mParentSegID);
        } while (0 != s1->mParentSegID);

twodone:
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
	debug("split", "\tparent = %u\n", parent->getValue());

	parent->segmentsJoinedIntoMe.erase( child->getValue() );
        mGraph->get( child->mValue )->cut();
	child->mParentSegID = 0;

	const float oldChildThreshold = child->mThreshold;
	child->mThreshold = 0;

	if( isSegmentSelected( parent->getValue() ) ){
		debug("split", "parent was selected\n");
		mSelectedSet.insert( child->getValue() );
	} else {
		mSelectedSet.remove( child->getValue() );
	}

	OmSegmentQueueElement parentElement = { child->mValue, oldChildThreshold };
	parent->queue.push(parentElement);

	OmSegmentQueueElement childElement = { parent->mValue, oldChildThreshold };
	child->queue.push(childElement);

	++mNumTopLevelSegs;

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

void OmSegmentCacheImpl::loadDendrogram()
{
	doLoadDendrogram( mSegmentation->mDend->getQuint32Ptr(), 
			  mSegmentation->mDendValues->getFloatPtr(), 
			  mSegmentation->mDendCount, 
			  mSegmentation->mDendThreshold);
}

void OmSegmentCacheImpl::doLoadDendrogram( const quint32 * dend, const float * dendValues, 
					   const int size, const float stopPoint )
{
	initializeDynamicTree();

	mNumTopLevelSegs = mNumSegs;

	quint32 joinCounter = 0;
	unsigned int childVal;
	unsigned int parentVal;
	float threshold;

	for(int i = 0; i < size; ++i) {
                childVal = dend[i];
		parentVal = dend[i + size ];
                threshold = dendValues[i];

                OmSegmentEdge * edge = Join(parentVal, childVal, threshold);
		edgeList.append( edge );

                ++joinCounter;
        }

	printf("loaded entire MST: %d joins performed\n", joinCounter );

	resetGlobalThreshold( stopPoint );

	clearCaches();
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

OmSegmentEdge * OmSegmentCacheImpl::Join(OmSegment * parent, OmSegment * childUnknownLevel, const float threshold)
{
	return Join( parent->getValue(), childUnknownLevel->getValue(), threshold );
}

OmSegmentEdge * OmSegmentCacheImpl::Join( const OmSegID parentID, const OmSegID childUnknownDepthID, const float threshold)
{
	loadTreeIfNeeded();

	DynamicTree<OmSegID> * childRootDT = mGraph->get( childUnknownDepthID )->findRoot();
	childRootDT->join( mGraph->get( parentID ) );

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( parentID );

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, threshold);

        if( isSegmentSelected( childUnknownDepthID ) ){
                mSelectedSet.insert( parent->mValue );
        } 
	mSelectedSet.remove( childUnknownDepthID );

	--mNumTopLevelSegs;

	return new OmSegmentEdge( parentID, childRoot->mValue, threshold);
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
		Join( parentID, *iter, 0 );
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

		if( counter > numToGet ){
			break;
		}
	}
	
	return ret;
}
