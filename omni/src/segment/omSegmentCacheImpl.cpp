#include "segment/omSegmentCacheImpl.h"
#include "utility/omHdf5Path.h"
#include "utility/omDataArchiveSegment.h"
#include "utility/omDataArchiveValue.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "utility/omDataPaths.h"

static const quint32 divSize = 10000;

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImpl::OmSegmentCacheImpl(OmSegmentation *segmentation, OmSegmentCache * cache )
	: mSegmentation(segmentation), mParentCache( cache )
{
	mMaxSegmentID = 0;
	mMaxValue = 0;
        mAllSelected = false;
        mAllEnabled = false;

	amInBatchMode = false;
	needToFlush = false;
}

OmSegmentCacheImpl::~OmSegmentCacheImpl()
{
	flushDirtySegments();
}

OmId OmSegmentCacheImpl::getSegmentationID()
{
	return mSegmentation->GetId();
}

OmSegment* OmSegmentCacheImpl::AddSegment()
{
	return AddSegment( getNextValue() );
}

inline quint32 OmSegmentCacheImpl::getSegmentPageNum( const OmId segID )
{
	return segID / divSize;
}

inline quint32 OmSegmentCacheImpl::getValuePageNum( const SEGMENT_DATA_TYPE value )
{
	return value / divSize;
}

OmSegment* OmSegmentCacheImpl::AddSegment(SEGMENT_DATA_TYPE value)
{
	OmId segID = getNextSegmentID();

	quint32 segPageNum = getSegmentPageNum(segID);
	quint32 valuePageNum = getValuePageNum(value);

	OmSegment* seg = new OmSegment( segID, value, mParentCache);
	mSegIdToSegPtrHash[ segPageNum ][segID] = seg;
	mValueToSegIdHash[ valuePageNum ][value] = segID;

	if (mMaxValue < value) {
		mMaxValue = value;
	}

	addToDirtySegmentList(seg);

	return seg;
}

void OmSegmentCacheImpl::AddSegmentsFromChunk(const SegmentDataSet & data_values, 
					      const OmMipChunkCoord & mipCoord )
{
	foreach( SEGMENT_DATA_TYPE value, data_values ){

		OmSegment * seg = GetSegmentFromValue( value );

                if( NULL == seg ) {
			seg = AddSegment( value );
		}

		seg->updateChunkCoordInfo( mipCoord );
		addToDirtySegmentList(seg);
        }
}

bool OmSegmentCacheImpl::doesValuePageExist( const quint32 valuePageNum )
{
	OmHdf5Path path = OmDataPaths::getValuePagePath( getSegmentationID(), valuePageNum );
	return OmProjectData::GetProjectDataReader()->dataset_exists( path );
}

bool OmSegmentCacheImpl::isValueAlreadyMappedToSegment( SEGMENT_DATA_TYPE value )
{
	if (0 == value) {	// Can't look up value 0 or hdf5 goes crazy in the head.
		return false;
	}

	const quint32 valuePageNum = getValuePageNum(value);

	if( !mValueToSegIdHash.contains( valuePageNum ) ){
		if( !doesValuePageExist( valuePageNum ) ){
			return false;
		}
		LoadValuePage( valuePageNum );
	}

	if( mValueToSegIdHash.value( valuePageNum ).contains( value ) ){
		return true;
	}

	return false;
}

OmId OmSegmentCacheImpl::getNextSegmentID()
{
	mMaxSegmentID++;
	return mMaxSegmentID;
}

SEGMENT_DATA_TYPE OmSegmentCacheImpl::getNextValue()
{
	mMaxValue++;
	return mMaxValue;
}

bool OmSegmentCacheImpl::IsSegmentValid(OmId seg)
{
        if (seg < 1 || seg > mMaxSegmentID) {
                return false;
        }
	return true;
}

OmSegment* OmSegmentCacheImpl::GetSegmentFromValue(SEGMENT_DATA_TYPE value)
{
	if ( !isValueAlreadyMappedToSegment( value ) ){
		return NULL;
	}

	const quint32 valuePageNum = getValuePageNum(value);

	if (!mValueToSegIdHash.contains(valuePageNum)) { // page should already be there...
		LoadValuePage( valuePageNum );
	}

	OmId segmentID = mValueToSegIdHash.value(valuePageNum).value(value);

	return GetSegmentFromID(segmentID);
}

OmSegment* OmSegmentCacheImpl::GetSegmentFromID(OmId segmentID)
{
	if ( !IsSegmentValid( segmentID ) ){
		return NULL;
	}

	const quint32 pageNum = getSegmentPageNum(segmentID);

	if (!mSegIdToSegPtrHash.contains( pageNum )){
		LoadSegmentPage( pageNum );
	}

	return mSegIdToSegPtrHash.value(pageNum).value(segmentID);
}

OmId OmSegmentCacheImpl::GetNumSegments()
{
	return mMaxSegmentID;
}

OmId OmSegmentCacheImpl::GetNumTopSegments()
{
	OmId num = 0;
	for( OmId i = 1; i < mMaxSegmentID; ++i ){
		if( GetSegmentFromID(i)->getParent() == 0 ){
			++num;
		}
	}
	return num;
}

quint32 OmSegmentCacheImpl::numberOfSelectedSegments()
{
	return mSelectedSet.size();
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
	if( mAllEnabled ||
	    mEnabledSet.contains( segID ) ){
		return true;
	}

	return false;
}

bool OmSegmentCacheImpl::isSegmentSelected( OmId segID )
{
	if( mAllSelected ||
	    mSelectedSet.contains( segID ) ){
		return true;
	}

	return false;
}

void OmSegmentCacheImpl::setSegmentEnabled( OmId segID, bool isEnabled )
{
	if (isEnabled) {
		mEnabledSet.unite( getIDs( GetSegmentFromID(segID)) );
	} else {
		mEnabledSet.subtract( getIDs( GetSegmentFromID(segID) ) );
	}
}

void OmSegmentCacheImpl::setSegmentSelected( OmId segID, bool isSelected )
{
	cacheOfSelectedSegmentValues.clear();

	if (isSelected) {
		mSelectedSet.unite( getIDs( GetSegmentFromID(segID) ) );
	} else {
		mSelectedSet.subtract( getIDs( GetSegmentFromID(segID) ) );
	}
}

SegmentDataSet OmSegmentCacheImpl::GetSelectedSegmentValues()
{
	if( !cacheOfSelectedSegmentValues.empty() ){
		return cacheOfSelectedSegmentValues;
	}

	SegmentDataSet values;

	foreach( OmId segID, mSelectedSet ){
		values.unite( getValues(GetSegmentFromID( segID )) );
	}

	cacheOfSelectedSegmentValues = values;

	return values;
}

SegmentDataSet OmSegmentCacheImpl::GetEnabledSegmentValues()
{
	SegmentDataSet values;

	foreach( OmId segID, mEnabledSet ){
		values.unite( getValues(GetSegmentFromID( segID ) ) );
	}

	return values;
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
		dirtySegments.insert(seg);
		
		if( dirtySegments.size() > maxDirtySegmentsBeforeFlushing() ){
			flushDirtySegments();
		}
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

		SavePages();

		time(&time_end);
		time_dif = difftime(time_end, time_start);

		printf("done (%.2lf secs)\n", time_dif);

		needToFlush = false;
	} else {

		SavePages();
		
		dirtySegments.clear();
	}
}

int OmSegmentCacheImpl::maxDirtySegmentsBeforeFlushing()
{
	return 1000;
}

SegmentDataSet OmSegmentCacheImpl::getValues( OmSegment * segment )
{
	OmSegment * root = findRoot( segment );

	if( !cacheRootNodeToAllChildrenValues.contains( root->GetId() )){
		cacheRootNodeToAllChildrenValues.insert( root->GetId(), getValuesHelper( root ) );
	}

	return cacheRootNodeToAllChildrenValues.value( root->GetId() );

}

SegmentDataSet OmSegmentCacheImpl::getValuesHelper( OmSegment * segment )
{
	SegmentDataSet values;

	foreach( OmId segID, getIDs( segment ) ){
		values.insert( GetSegmentFromID(segID)->getValue() );
	}

	return values;
}

OmIds OmSegmentCacheImpl::getIDs( OmSegment * segment )
{
	OmSegment * root = findRoot( segment );

	if( !cacheRootNodeToAllChildrenIDs.contains( root->GetId() )){
		cacheRootNodeToAllChildrenIDs.insert( root->GetId(), getIDsHelper( root ) );
	}

	return cacheRootNodeToAllChildrenIDs.value( root->GetId() );
}

OmIds OmSegmentCacheImpl::getIDsHelper( OmSegment * segment )
{
	OmIds ids;
	ids.insert( segment->GetId() );

	foreach( OmId segID, segment->segmentsJoinedIntoMe ){
		ids.unite( getIDsHelper( GetSegmentFromID(segID)) );
	}

	return ids;
}

OmSegment * OmSegmentCacheImpl::findRoot( OmSegment * segment )
{
	OmSegment * root = segment;
	while( root->getParent() != 0 ){
		root = GetSegmentFromID( root->getParent() );
	}

	return GetSegmentFromID(root->GetId());
}

void OmSegmentCacheImpl::splitChildLowestThreshold( OmSegment * segmentUnknownLevel )
{
	OmSegment * root = findRoot(segmentUnknownLevel);

	double minThreshold = 1;
	OmSegment * segToRemove = NULL;
	foreach( OmId childID, root->segmentsJoinedIntoMe ){
		OmSegment * child = GetSegmentFromID( childID );
		if( child->getThreshold() < minThreshold){
			minThreshold = child->getThreshold();
			segToRemove = child;
		}
	}
	
	if( NULL == segToRemove ){
		printf("no children to remove\n");
		return;
	}

	clearCaches( root );

	root->removeChild( segToRemove );
	segToRemove->clearParent();

	printf("removed %d from parent\n", segToRemove->GetId() );
}

void OmSegmentCacheImpl::clearCaches( OmSegment * seg )
{
	cacheOfSelectedSegmentValues.clear();
	cacheRootNodeToAllChildrenIDs.remove( seg->GetId() );
	cacheRootNodeToAllChildrenValues.remove( seg->GetId() );
}

void OmSegmentCacheImpl::SavePages()
{
	// TODO: only save dirty pages

	foreach( quint32 segPageNum, mSegIdToSegPtrHash.keys() ){
		QHash<OmId, OmSegment*> page = mSegIdToSegPtrHash.value( segPageNum );
		OmDataArchiveSegment::ArchiveWrite( OmDataPaths::getSegmentPagePath( getSegmentationID(), segPageNum ),
					       page );
	}

	foreach( quint32 valuePageNum, mValueToSegIdHash.keys() ){
		QHash<SEGMENT_DATA_TYPE, OmId> page = mValueToSegIdHash.value( valuePageNum );
		OmDataArchiveValue::ArchiveWrite( OmDataPaths::getValuePagePath( getSegmentationID(), valuePageNum ),
					       page );
	}
}

void OmSegmentCacheImpl::turnBatchModeOn( const bool batchMode )
{
	amInBatchMode = batchMode;

	if( batchMode ){
		printf("segment cache batch mode on\n");
	} else {
		printf("segment cache batch mode off\n");
	}
}

void OmSegmentCacheImpl::LoadSegmentPage( const quint32 segPageNum )
{
	QHash<OmId, OmSegment*> page;
	OmDataArchiveSegment::ArchiveRead( OmDataPaths::getSegmentPagePath( getSegmentationID(), segPageNum ),
				      page );
	foreach( OmSegment * seg, page ){
		seg->mCache = mParentCache;
	}
	mSegIdToSegPtrHash[ segPageNum ] = page;
}

void OmSegmentCacheImpl::LoadValuePage( const quint32 valuePageNum )
{
	QHash<SEGMENT_DATA_TYPE, OmId> page;
	OmDataArchiveValue::ArchiveRead( OmDataPaths::getValuePagePath( getSegmentationID(), valuePageNum ),
				      page );
	mValueToSegIdHash[ valuePageNum ] = page;
}
