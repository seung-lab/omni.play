#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "system/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImplLowLevel::OmSegmentCacheImplLowLevel( OmSegmentation * segmentation, OmSegmentCache * cache )
	: mSegmentation(segmentation)
	, mParentCache( cache )
	, mSegments( new OmPagingStore<OmSegment>( segmentation, cache ) )
	, mMaxValue(0)
	, mNumSegs(0)
	, mAllSelected(false)
	, mAllEnabled(false)
{
}

OmSegmentCacheImplLowLevel::~OmSegmentCacheImplLowLevel()
{
	delete mSegments;
}

OmSegment * OmSegmentCacheImplLowLevel::findRoot( OmSegment * segment )
{
	if(0 == segment->mParentSegID) {
		return segment;
	}
	
	return GetSegmentFromValue( mSegmentGraph.graph_getRootID( segment->mValue ) );
}

OmSegID OmSegmentCacheImplLowLevel::findRootID( const OmSegID segID )
{
	return findRoot( GetSegmentFromValue( segID ) )->getValue();
}

bool OmSegmentCacheImplLowLevel::isValueAlreadyMappedToSegment(const OmSegID value)
{
	return mSegments->IsValueAlreadyMapped( value );
}

OmSegment* OmSegmentCacheImplLowLevel::GetSegmentFromValue(const OmSegID value ) 
{
	return mSegments->GetSegmentFromValue( value );
}

OmSegID OmSegmentCacheImplLowLevel::GetNumSegments()
{
	return mNumSegs;
}

OmSegID OmSegmentCacheImplLowLevel::GetNumTopSegments()
{
	return mSegmentGraph.getNumTopLevelSegs();
}

bool OmSegmentCacheImplLowLevel::isSegmentEnabled( OmSegID segID )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

	if( mAllEnabled ||
	    mEnabledSet.contains( rootID ) ){
		return true;
	}

	return false;
}

void OmSegmentCacheImplLowLevel::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();
	clearCaches();

	if (isEnabled) {
		mEnabledSet.insert( rootID );
	} else {
		mEnabledSet.remove( rootID );
	}
}

void OmSegmentCacheImplLowLevel::SetAllEnabled(bool enabled)
{
	mAllEnabled = enabled;
	mEnabledSet.clear();
}

OmSegIDsSet & OmSegmentCacheImplLowLevel::GetEnabledSegmentIdsRef()
{
        return mEnabledSet;
}
 
bool OmSegmentCacheImplLowLevel::isSegmentSelected( OmSegment * seg )
{
	const OmSegID rootID = findRoot( seg )->getValue();

	if( mAllSelected ||
	    mSelectedSet.contains( rootID ) ){
		return true;
	}

	return false;
}
 
bool OmSegmentCacheImplLowLevel::isSegmentSelected( OmSegID segID )
{
 	return isSegmentSelected( GetSegmentFromValue( segID ) );
}

void OmSegmentCacheImplLowLevel::SetAllSelected(bool selected)
{
        mAllSelected = selected;
 	mSelectedSet.clear();
}
 
void OmSegmentCacheImplLowLevel::setSegmentSelected( OmSegID segID, bool isSelected )
{
	setSegmentSelectedBatch( segID, isSelected );
	clearCaches();
} 

void OmSegmentCacheImplLowLevel::UpdateSegmentSelection( const OmSegIDsSet & ids )
{
	mSelectedSet.clear();

	OmSegIDsSet::const_iterator iter;
	for( iter = ids.begin(); iter != ids.end(); ++iter ){
		setSegmentSelectedBatch( *iter, true );
	}

	clearCaches();	
}
 
void OmSegmentCacheImplLowLevel::setSegmentSelectedBatch( OmSegID segID, bool isSelected )
{
       const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

       if (isSelected) {
               doSelectedSetInsert( rootID );
       } else {
               doSelectedSetRemove( rootID );
               assert( !mSelectedSet.contains( segID ));
       }
}

OmSegIDsSet & OmSegmentCacheImplLowLevel::GetSelectedSegmentIdsRef()
{
        return mSelectedSet;
} 
 
quint32 OmSegmentCacheImplLowLevel::numberOfSelectedSegments()
{
	return mSelectedSet.size();
}
 
bool OmSegmentCacheImplLowLevel::AreSegmentsSelected()
{
	if( 0 == numberOfSelectedSegments() ){
		return false;
	}

	return true;
}

void OmSegmentCacheImplLowLevel::doSelectedSetInsert( const OmSegID segID)
{
	mSelectedSet.insert( segID );
	addToRecentMap(segID);
}
 		
void OmSegmentCacheImplLowLevel::doSelectedSetRemove( const OmSegID segID)
{
	mSelectedSet.remove( segID );
	addToRecentMap(segID);
}
	
void OmSegmentCacheImplLowLevel::addToRecentMap( const OmSegID segID )
{
	mRecentRootActivityMap.touch( segID );
}

QString OmSegmentCacheImplLowLevel::getSegmentName( OmSegID segID )
{
	if( segmentCustomNames.contains(segID ) ){
		return segmentCustomNames.value(segID);
	}

	return ""; //QString("segment%1").arg(segID);
}

void OmSegmentCacheImplLowLevel::setSegmentName( OmSegID segID, QString name )
{
	segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImplLowLevel::getSegmentNote( OmSegID segID )
{
	if( segmentNotes.contains(segID ) ){
		return segmentNotes.value(segID);
	}

	return "";
}

void OmSegmentCacheImplLowLevel::setSegmentNote( OmSegID segID, QString note )
{
	segmentNotes[ segID ] = note;
}

OmSegID OmSegmentCacheImplLowLevel::getSegmentationID()
{
	return mSegmentation->GetId();
}

void OmSegmentCacheImplLowLevel::addToDirtySegmentList( OmSegment* seg)
{
	mSegments->AddToDirtyList( seg->mValue );
}

void OmSegmentCacheImplLowLevel::flushDirtySegments()
{
	mSegments->FlushDirtyItems();
}

void OmSegmentCacheImplLowLevel::turnBatchModeOn( const bool batchMode )
{
	mSegments->SetBatchMode(batchMode);
}

quint32 OmSegmentCacheImplLowLevel::getPageSize() 
{ 
	return mSegments->getPageSize(); 
}

quint32 OmSegmentCacheImplLowLevel::getMaxValue()
{ 
	return mMaxValue; 
}

///////////////////////////
////// Private 
///////////////////////////

OmSegID OmSegmentCacheImplLowLevel::getNextValue()
{
	++mMaxValue;
	return mMaxValue;
}

void OmSegmentCacheImplLowLevel::clearCaches()
{
	OmCacheManager::Freshen(true);
}
