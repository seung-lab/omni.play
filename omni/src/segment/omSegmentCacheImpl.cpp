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

        mAllSelected = false;
        mAllEnabled = false;

	amInBatchMode = false;
	needToFlush = false;
	mPageSize = 10000;
	mAllPagesLoaded = false;

	mCachedRootFreshness = 1;
	mCachedColorFreshness = 1;

	tree = NULL;
}

OmSegmentCacheImpl::~OmSegmentCacheImpl()
{
	foreach( const quint32 & pageNum, validPageNumbers ){

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
	assert(0);
	/*
	OmId num = 0;
	for( OmId i = 1; i <= mMaxSegmentID; ++i ){
		if( GetSegmentFromID(i)->getParent() == 0 ){
			++num;
		}
	}
	return num;
	*/
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
        return mSelectedSet;
}

OmIds & OmSegmentCacheImpl::GetEnabledSegmentIdsRef()
{
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
		mSelectedSet.remove( segID ); // FIXME: this shouldn't be necessary....(purcaro)
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

OmSegment * OmSegmentCacheImpl::findRoot( OmSegment * segment )
{
	if(0 == segment->parentSegID) {
		return segment;
	}

	OmSegment * node = segment;

	while( NULL != node->mCachedRoot &&
	       node->mCachedRootFreshness == mCachedRootFreshness ){
		node = node->mCachedRoot;
	}

	//	int counter = 0;
	while ( 0 != node->parentSegID ){

		if( NULL == node->mParent ){
			node->mParent = GetSegmentFromValueFast( node->parentSegID );
		}

		node = node->mParent;
		//	++counter;
	}
	//	printf("counter: %d\n", counter);

	segment->mCachedRoot = node;
	segment->mCachedRootFreshness = mCachedRootFreshness;

	return node;
}

void OmSegmentCacheImpl::splitTwoChildren(OmSegment * seg1, OmSegment * seg2)
{
	if( findRoot(seg1) != findRoot(seg2) ){
		debug("dend", "can't split disconnected objects.\n");
		return;
	}

	OmSegment * seg;
	if(seg1->getThreshold() > seg2->getThreshold()){
		seg = seg1;
	} else {
		seg = seg2;
	}

	debug("dend", "splitting off %d\n", seg->getValue());
	
	splitChildFromParent( seg );
	clearCaches();
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
	clearCaches();
}

void OmSegmentCacheImpl::splitChildFromParent( OmSegment * child )
{
	if( !child->parentSegID ){
		return;
	}

	OmSegment * parent = child->mParent;

	parent->segmentsJoinedIntoMe.removeAll( child->getValue() );
	child->parentSegID = 0;
	child->mParent = NULL;
	child->mThreshold = 0;
	
	if( isSegmentEnabled( parent->getValue() ) ){
		mSelectedSet.insert( child->getValue() );
	}

	invalidateCachedRootFreshness();
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
			seg->parentSegID = 0;
			seg->mThreshold = 0;
			seg->segmentsJoinedIntoMe.clear();
			seg->mParent = NULL;
			seg->mCachedRoot = NULL;
			seg->mCachedRootFreshness = 0;
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

void OmSegmentCacheImpl::Join(OmSegment * parent, OmSegment * childUnknownLevel, double threshold)
{

	//	JoinDynamicTree( parent->getValue(), childUnknownLevel->getValue(), threshold );
	//	return;

	OmSegment * childRoot = findRoot( childUnknownLevel );
	OmSegment * parentRoot = findRoot( parent );

	if(childRoot->getValue() == parentRoot->getValue() ){
		printf("skipping join operation: child %d already joined with root %d\n", childUnknownLevel->getValue(), parentRoot->getValue());
		return;
	}
		
	if( parent->segmentsJoinedIntoMe.contains( childRoot->getValue() ) ){
		assert(0);
	}
	parent->segmentsJoinedIntoMe.append( childRoot->getValue() );
	childRoot->setParent(parent, threshold);

	childRoot->mCachedRoot = parentRoot;
	childRoot->mCachedRootFreshness = mCachedRootFreshness;

	childUnknownLevel->mCachedRoot = parentRoot;
	childUnknownLevel->mCachedRootFreshness = mCachedRootFreshness;

	addToDirtySegmentList(parent);
	addToDirtySegmentList(childRoot);
}

extern bool mShatter;
OmColor OmSegmentCacheImpl::getVoxelColorForView2d( const SEGMENT_DATA_TYPE val, const bool showOnlySelectedSegments )
{
	OmColor color = {0,0,0};

	OmSegment * seg = GetSegmentFromValue( val );
	if( NULL == seg ) {
		return color;
	}

	if(mShatter){
		OmSegment * segRoot = findRoot( seg );
		if( isSegmentSelected(segRoot)) {
                	color.red   = seg->mColor.x * 255;
                	color.green = seg->mColor.y * 255;
                	color.blue  = seg->mColor.z * 255;
			return color;
		} 
	}
	

	if(mCachedColorFreshness == seg->mCachedColorFreshness ){
		return seg->mCachedColor;
	}

	OmSegment * segRoot = findRoot( seg );

	const Vector3 < float > & sc = segRoot->mColor;

	if( isSegmentSelected(segRoot)) {
		color.red   = clamp(sc.x * 255 * 2.5);
		color.green = clamp(sc.y * 255 * 2.5);
		color.blue  = clamp(sc.z * 255 * 2.5);
	} else {
		if (showOnlySelectedSegments) {
			// don't show
		} else {
			color.red   = sc.x * 255;
			color.green = sc.y * 255;
			color.blue  = sc.z * 255;
		}
	}

	seg->mCachedColor = color;
	seg->mCachedColorFreshness = mCachedColorFreshness;

	return color;
}

void OmSegmentCacheImpl::invalidateCachedRootFreshness()
{
	mCachedRootFreshness++;
}

void OmSegmentCacheImpl::invalidateCachedColorFreshness()
{
	mCachedColorFreshness++;
}

void OmSegmentCacheImpl::reloadDynamicTree()
{
	if( NULL != tree ){
		delete [] tree;
	}

	tree = new DynamicTree<SEGMENT_DATA_TYPE> * [mMaxValue];

	for( quint32 i = 0; i <= mMaxValue; ++i ){
		tree[ i ] = DynamicTree<SEGMENT_DATA_TYPE>::makeTree( i );
	}
}

void OmSegmentCacheImpl::reloadDendrogram( const quint32 * dend, const float * dendValues, 
					   const int size, const float stopPoint )
{
	debug("dend", "dend size=%i\n", size);
	turnBatchModeOn(true);

	clearAllJoins();

	quint32 counter = 0;
	for(int i = 0; i < size; ++i) {
                const unsigned int childVal = dend[i];
                const unsigned int parentVal = dend[i + size ];
                const float threshold = dendValues[i];

		if(threshold < stopPoint)  {
			break;
		}

                OmSegment * child = GetSegmentFromValue(childVal);
                OmSegment * parent = GetSegmentFromValue(parentVal);

                if (NULL == child || NULL == parent) {
                        printf( "Not joining %d, %d because of NULL condition\n", childVal, parentVal);
                        continue;
                }

		//printf("joining child %d to parent %d....\n", childVal, parentVal);
                Join(parent, child, threshold);
                ++counter;
                if( counter % 1000 == 0 ){
                        printf("\t%d join operations...\n", counter);
		}
        }
	
	flushDirtySegments();
	turnBatchModeOn(false);
}

void OmSegmentCacheImpl::JoinDynamicTree( const OmId parentID, const OmId childUnknownDepthID, const double threshold)
{
	reloadDynamicTree();

	DynamicTree<SEGMENT_DATA_TYPE> * childRootDT = tree[ childUnknownDepthID ]->findRoot();
	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );

	DynamicTree<SEGMENT_DATA_TYPE> * parentRootDT = tree[ parentID ]->findRoot();
	OmSegment * parentRoot = GetSegmentFromValue( parentRootDT->getKey() );

	if(childRoot->getValue() == parentRoot->getValue() ){
		printf("skipping join operation: %d joined with %d\n", childRoot->getValue(), parentRoot->getValue());
		return;
	}

	printf("get key is: %d\n", childRootDT->getKey() );

	childRootDT->join( tree[ parentID ] );

	OmSegment * childUnknownLevel = GetSegmentFromValue( childUnknownDepthID );
	OmSegment * parent = GetSegmentFromValue( parentID );

	parent->segmentsJoinedIntoMe.append( childRoot->getValue() );
	childRoot->setParent(parent, threshold);

	childRoot->mCachedRoot = parentRoot;
	childRoot->mCachedRootFreshness = mCachedRootFreshness;

	childUnknownLevel->mCachedRoot = parentRoot;
	childUnknownLevel->mCachedRootFreshness = mCachedRootFreshness;

	addToDirtySegmentList(parent);
	addToDirtySegmentList(childRoot);
}

