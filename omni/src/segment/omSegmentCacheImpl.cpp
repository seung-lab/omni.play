#include "segment/omSegmentCacheImpl.h"
#include "utility/omHdf5Path.h"
#include "utility/omDataArchiveSegment.h"
#include "system/omProjectData.h"
#include "system/omCacheManager.h"
#include "volume/omSegmentation.h"
#include "utility/omDataPaths.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation *segmentation, OmSegmentCache * cache )
	: mSegmentation(segmentation), mParentCache( cache )
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

	mCachedColorFreshness = 1;

	mTree = NULL;
}

OmSegmentCacheImpl::~OmSegmentCacheImpl()
{
	foreach( const quint32 & pageNum, loadedPageNumbers ){

		for( quint32 i = 0; i < mPageSize; ++i ){
			delete mValueToSegPtrHash.value(pageNum)[i];
		}

		delete [] mValueToSegPtrHash[pageNum];
	}
}

OmId OmSegmentCacheImpl::getSegmentationID()
{
	return mSegmentation->GetId();
}

OmSegment* OmSegmentCacheImpl::AddSegment()
{
	return AddSegment( getNextValue() );
}

inline quint32 OmSegmentCacheImpl::getValuePageNum( const SEGMENT_DATA_TYPE value )
{
	return value / mPageSize;
}

OmSegment* OmSegmentCacheImpl::AddSegment(SEGMENT_DATA_TYPE value)
{
	quint32 valuePageNum = getValuePageNum(value);

	OmSegment* seg = new OmSegment( value, mParentCache);
	if( !validPageNumbers.contains(valuePageNum)) {
		mValueToSegPtrHash[ valuePageNum ] = new OmSegment*[mPageSize];
		memset(mValueToSegPtrHash[ valuePageNum ], '\0', sizeof(OmSegment*) * mPageSize);
		validPageNumbers.insert( valuePageNum );
		loadedPageNumbers.insert( valuePageNum );
	}
	mValueToSegPtrHash[ valuePageNum ][value % mPageSize] = seg;

	++mNumSegs;
	++mNumTopLevelSegs;

	if (mMaxValue < value) {
		mMaxValue = value;
	}

	addToDirtySegmentList(seg);

	return seg;
}

void OmSegmentCacheImpl::AddSegmentsFromChunk(const SegmentDataSet & data_values, 
					      const OmMipChunkCoord & )
{
	foreach( const SEGMENT_DATA_TYPE & value, data_values ){

		OmSegment * seg = GetSegmentFromValue( value );

                if( NULL == seg ) {
			seg = AddSegment( value );
		}

		addToDirtySegmentList(seg);
        }
}

bool OmSegmentCacheImpl::isValueAlreadyMappedToSegment( SEGMENT_DATA_TYPE value )
{
	if (0 == value) {
		return false;
	}

	const quint32 valuePageNum = getValuePageNum(value);

	if( !validPageNumbers.contains( valuePageNum ) ){
		return false;
	}

	if( !loadedPageNumbers.contains( valuePageNum ) ){
		LoadValuePage( valuePageNum );
	}

	if( NULL != mValueToSegPtrHash.value(valuePageNum)[value % mPageSize]){
		return true;
	}

	return false;
}

SEGMENT_DATA_TYPE OmSegmentCacheImpl::getNextValue()
{
	mMaxValue++;
	return mMaxValue;
}

OmSegment* OmSegmentCacheImpl::GetSegmentFromValue(SEGMENT_DATA_TYPE value)
{
	if ( !isValueAlreadyMappedToSegment( value ) ){
		return NULL;
	}

	const quint32 valuePageNum = getValuePageNum(value);

	return mValueToSegPtrHash.value( valuePageNum )[ value % mPageSize];
}

inline OmSegment* OmSegmentCacheImpl::GetSegmentFromValueFast(SEGMENT_DATA_TYPE value)
{
	if( !mAllPagesLoaded ){
		if ( !isValueAlreadyMappedToSegment( value ) ){
			return NULL;
		}
	}

	return mValueToSegPtrHash.value( value / mPageSize )[ value % mPageSize];
}

OmId OmSegmentCacheImpl::GetNumSegments()
{
	return mNumSegs;
}

OmId OmSegmentCacheImpl::GetNumTopSegments()
{
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

OmIds & OmSegmentCacheImpl::GetSelectedSegmentIdsRef()
{
	loadTreeIfNeeded();
        return mSelectedSet;
}

OmIds & OmSegmentCacheImpl::GetEnabledSegmentIdsRef()
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

bool OmSegmentCacheImpl::isSegmentEnabled( OmId segID )
{
	OmId rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

	if( mAllEnabled ||
	    mEnabledSet.contains( rootID ) ){
		return true;
	}

	return false;
}

bool OmSegmentCacheImpl::isSegmentSelected( OmId segID )
{
	return isSegmentSelected( GetSegmentFromValue( segID ) );
}

bool OmSegmentCacheImpl::isSegmentSelected( OmSegment * seg )
{
	OmId rootID = findRoot( seg )->getValue();

	if( mAllSelected ||
	    mSelectedSet.contains( rootID ) ){
		return true;
	}

	return false;
}

void OmSegmentCacheImpl::setSegmentEnabled( OmId segID, bool isEnabled )
{
	OmId rootID = findRoot( GetSegmentFromValue(segID) )->getValue();
	clearCaches();

	if (isEnabled) {
		mEnabledSet.insert( rootID );
	} else {
		mEnabledSet.remove( rootID );
	}
}

void OmSegmentCacheImpl::setSegmentSelected( OmId segID, bool isSelected )
{
	OmId rootID = findRoot( GetSegmentFromValue(segID) )->getValue();
	clearCaches();

	if (isSelected) {
		mSelectedSet.insert( rootID );
	} else {
		mSelectedSet.remove( rootID );
		mSelectedSet.remove( segID );
	}
}

void OmSegmentCacheImpl::setSegmentName( OmId segID, QString name )
{
	segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImpl::getSegmentName( OmId segID )
{
	if( segmentCustomNames.contains(segID ) ){
		return segmentCustomNames.value(segID);
	}

	return QString("segment%1").arg(segID);
}

void OmSegmentCacheImpl::setSegmentNote( OmId segID, QString note )
{
	segmentNotes[ segID ] = note;
}

QString OmSegmentCacheImpl::getSegmentNote( OmId segID )
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

		time_t time_start;
		time_t time_end;
		double time_dif;

		printf("flushing all segment metadata; please wait...");
		time(&time_start);

		SaveAllPages();

		time(&time_end);
		time_dif = difftime(time_end, time_start);

		printf("done (%.2lf secs)\n", time_dif);

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
					if(0 != small1->mParentSegID) {
						splitChildFromParent(small1);
					} else {
						splitChildFromParent(small2);
					}
				} else {
					if(0 != small2->mParentSegID) {
						splitChildFromParent(small2);
					} else {
						splitChildFromParent(small1);
					}
				}
				return;
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
}

void OmSegmentCacheImpl::splitChildLowestThreshold( OmSegment * segmentUnknownLevel )
{
	OmSegment * root = findRoot(segmentUnknownLevel);

	double minThreshold = 1;
	OmSegment * segToRemove = NULL;
	foreach( const OmId & childID, root->segmentsJoinedIntoMe ){
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
	debug("split", "OmSegmentCacheImpl::splitChildFromParent=%u\n", child->getValue());
	if( !child->mParentSegID ){
		return;
	}

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );
	debug("split", "\tparent = %u\n", parent->getValue());

	parent->segmentsJoinedIntoMe.removeAll( child->getValue() );
        mTree->get( child->mValue )->cut();
	child->mParentSegID = 0;
	child->mThreshold = 0;
	
	if( isSegmentSelected( parent->getValue() ) ){
		debug("split", "parent was selected\n");
		mSelectedSet.insert( child->getValue() );
	} else {
		mSelectedSet.remove( child->getValue() );
	}

	clearCaches();
}

void OmSegmentCacheImpl::SaveAllPages()
{
	foreach( const quint32 & pageNum, loadedPageNumbers ){
		doSaveSegmentPage( pageNum );
	}
}

void OmSegmentCacheImpl::SaveDirtySegmentPages()
{
	foreach( const quint32 & segPageNum, dirtySegmentPages ){
		doSaveSegmentPage( segPageNum );
	}
}

void OmSegmentCacheImpl::doSaveSegmentPage( const quint32 segPageNum )
{
	OmSegment** page = mValueToSegPtrHash.value( segPageNum );
	OmDataArchiveSegment::ArchiveWrite( OmDataPaths::getSegmentPagePath( getSegmentationID(), segPageNum ),
					    page, mParentCache );
}

void OmSegmentCacheImpl::turnBatchModeOn( const bool batchMode )
{
	amInBatchMode = batchMode;
}

void OmSegmentCacheImpl::LoadValuePage( const quint32 segPageNum )
{
	OmSegment** page = new OmSegment*[mPageSize];
	memset(page, '\0', sizeof(OmSegment*) * mPageSize);

	OmDataArchiveSegment::ArchiveRead( OmDataPaths::getSegmentPagePath( getSegmentationID(), segPageNum ),
					   page,
					   mParentCache);
	
	mValueToSegPtrHash[ segPageNum ] = page;
	loadedPageNumbers.insert( segPageNum );

	if( loadedPageNumbers == validPageNumbers ){
		mAllPagesLoaded = true;
	}
}

void OmSegmentCacheImpl::clearAllJoins()
{
	printf("clearing old join information...");

	foreach( const quint32 & pageNum, loadedPageNumbers ){
		
		for( quint32 i = 0; i < mPageSize; ++i ){
			if( NULL == mValueToSegPtrHash.value(pageNum)[i] ){
				continue;
			}
			
			OmSegment * seg = mValueToSegPtrHash.value(pageNum)[i];
			seg->mParentSegID = 0;
			seg->mThreshold = 0;
			seg->segmentsJoinedIntoMe.clear();
		}
	}

	clearCaches();

	printf("done\n");
}

void OmSegmentCacheImpl::setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
						    QList< OmSegment* > segmentsToDraw )
{
	cacheDirectSegmentList[ chunkCoord ] = segmentsToDraw;
}

bool OmSegmentCacheImpl::segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord )
{
	return cacheDirectSegmentList.contains( chunkCoord );
}

QList< OmSegment* > OmSegmentCacheImpl::getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord )
{
	return cacheDirectSegmentList.value( chunkCoord );
}

void OmSegmentCacheImpl::clearCaches()
{
	OmCacheManager::Freshen(true);
	cacheDirectSegmentList.clear();
	invalidateCachedColorFreshness();
}

void OmSegmentCacheImpl::invalidateCachedColorFreshness()
{
	mCachedColorFreshness++;
}

void OmSegmentCacheImpl::initializeDynamicTree()
{
	if( NULL != mTree ){
		delete mTree;
	}

	mTree = new DynamicTreeContainer<SEGMENT_DATA_TYPE>( mMaxValue + 1); // mMaxValue is a valid segment
}

void OmSegmentCacheImpl::reloadDendrogram( const quint32 * dend, const float * dendValues, 
					   const int size, const float stopPoint )
{
	clearAllJoins();
	loadDendrogram( dend, dendValues, size, stopPoint );
}

void OmSegmentCacheImpl::doLoadDendrogram()
{
	loadDendrogram( mSegmentation->mDend, 
			mSegmentation->mDendValues, 
			mSegmentation->mDendCount, 
			mSegmentation->mDendThreshold);
}

void OmSegmentCacheImpl::loadDendrogram( const quint32 * dend, const float * dendValues, 
					 const int size, const float stopPoint )
{
	initializeDynamicTree();

	quint32 counter = 0;
	unsigned int childVal;
	unsigned int parentVal;
	float threshold;
	for(int i = 0; i < size; ++i) {
                threshold = dendValues[i];

		if(threshold < stopPoint)  {
			break;
		}

                childVal = dend[i];
		parentVal = dend[i + size ];

                Join(parentVal, childVal, threshold);

                ++counter;
        }

	clearCaches();

	printf("\t %d join operations performed\n", counter);
}

void OmSegmentCacheImpl::Join(OmSegment * parent, OmSegment * childUnknownLevel, float threshold)
{
	Join( parent->getValue(), childUnknownLevel->getValue(), threshold );
}

void OmSegmentCacheImpl::Join( const OmId parentID, const OmId childUnknownDepthID, const float threshold)
{
	loadTreeIfNeeded();

	DynamicTree<SEGMENT_DATA_TYPE> * childRootDT  = mTree->get( childUnknownDepthID )->findRoot();
	childRootDT->join( mTree->get( parentID ) );

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( parentID );

	parent->segmentsJoinedIntoMe.append( childRoot->mValue );
	childRoot->setParent(parent, threshold);

        if( isSegmentSelected( childUnknownDepthID ) ){
                mSelectedSet.insert( parent->mValue );
        } 
	mSelectedSet.remove( childUnknownDepthID );
	--mNumTopLevelSegs;
}

OmSegment * OmSegmentCacheImpl::findRoot( OmSegment * segment )
{
	loadTreeIfNeeded();

	if(0 == segment->mParentSegID) {
		return segment;
	}

	DynamicTree<SEGMENT_DATA_TYPE> * rootDT  = mTree->get( segment->mValue )->findRoot();
	
	return GetSegmentFromValue( rootDT->getKey() );
}

void OmSegmentCacheImpl::loadTreeIfNeeded()
{
	if( NULL != mTree ){
		return;
	}

	doLoadDendrogram();
}

void OmSegmentCacheImpl::JoinAllSegmentsInSelectedList()
{
	if( mSelectedSet.size() < 2 ){
		return;
	}

	OmIds set = mSelectedSet; // Join() will modify mSelectedSet

	OmIds::const_iterator iter = set.constBegin();
	SEGMENT_DATA_TYPE parentID = *iter;
	++iter;

	while (iter != set.constEnd()) {
		printf("joining %d to %d\n", parentID, *iter);
		Join( parentID, *iter, 0 );
		++iter;
	}

	clearCaches();
}

const Vector3<float> & OmSegmentCacheImpl::GetColorAtThreshold( OmSegment * segment, const float threshold )
{
	debug("colorizer", "%s: figuring out color...\n", __FUNCTION__);

	OmSegment * seg = segment;

	while( 0 != seg->mParentSegID && seg->mThreshold < threshold ){
		seg = GetSegmentFromValue( seg->mParentSegID );
	}

	return seg->mColor;
}
