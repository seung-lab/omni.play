#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "system/cache/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentLists.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImplLowLevel::OmSegmentCacheImplLowLevel(OmSegmentation * segmentation)
	: segmentation_(segmentation)
	, mSegments(new OmPagingPtrStore(segmentation))
	, mMaxValue(0)
	, mNumSegs(0)
	, mAllSelected(false)
	, mAllEnabled(false)
{
}

OmSegmentCacheImplLowLevel::~OmSegmentCacheImplLowLevel()
{
}

OmSegment * OmSegmentCacheImplLowLevel::findRoot( OmSegment * segment )
{
	if(!segment->getParent()) {
		return segment;
	}

	return GetSegment( mSegmentGraph.graph_getRootID( segment->value() ) );
}

OmSegment * OmSegmentCacheImplLowLevel::findRoot(const OmSegID segID)
{
	return findRoot(GetSegment(segID));
}

OmSegID OmSegmentCacheImplLowLevel::findRootID(const OmSegID segID)
{
	if(!segID){
		return 0;
	}

	return findRoot( GetSegment( segID ) )->value();
}

OmSegID OmSegmentCacheImplLowLevel::findRootID(OmSegment* segment)
{
	if(!segment){
		return 0;
	}

	return findRoot(segment)->value();
}

OmSegment* OmSegmentCacheImplLowLevel::GetSegment(const OmSegID value)
{
	OmSegment* seg = mSegments->GetSegment(value);

	if(seg && seg->value() > mMaxValue){
		throw OmIoException("bad segment value: "
							+ om::NumToStr(seg->value())
							+ "; maxValue is: "
							+ om::NumToStr(mMaxValue));
	}

	return seg;
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
	const OmSegID rootID = findRoot( GetSegment(segID) )->value();

	if( mAllEnabled ||
	    0 != mEnabledSet.count(rootID)){
		return true;
	}

	return false;
}

void OmSegmentCacheImplLowLevel::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	const OmSegID rootID = findRoot(GetSegment(segID))->value();
	clearCaches();

	if (isEnabled) {
		mEnabledSet.insert(rootID);
	} else {
		mEnabledSet.erase(rootID);
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
	const OmSegID rootID = findRoot( seg )->value();

	if( mAllSelected ||
	    0 != mSelectedSet.count(rootID) ){
		return true;
	}

	return false;
}

bool OmSegmentCacheImplLowLevel::isSegmentSelected( OmSegID segID )
{
 	return isSegmentSelected( GetSegment( segID ) );
}

void OmSegmentCacheImplLowLevel::SetAllSelected(bool selected)
{
        mAllSelected = selected;
 	mSelectedSet.clear();
}

void OmSegmentCacheImplLowLevel::setSegmentSelected( OmSegID segID,
						     const bool isSelected,
						     const bool addToRecentList)
{
	setSegmentSelectedBatch( segID, isSelected, addToRecentList );
	clearCaches();
}

void OmSegmentCacheImplLowLevel::UpdateSegmentSelection( const OmSegIDsSet & ids,
							 const bool addToRecentList)
{
	mSelectedSet.clear();

	FOR_EACH(iter, ids){
		setSegmentSelectedBatch( *iter, true, addToRecentList);
	}

	clearCaches();
}

void OmSegmentCacheImplLowLevel::setSegmentSelectedBatch( OmSegID segID,
							  const bool isSelected,
							  const bool addToRecentList)
{
       const OmSegID rootID = findRoot( GetSegment(segID) )->value();

       if (isSelected) {
               doSelectedSetInsert( rootID, addToRecentList);
       } else {
               doSelectedSetRemove( rootID );
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

uint32_t OmSegmentCacheImplLowLevel::numberOfEnabledSegments()
{
	return mEnabledSet.size();
}

bool OmSegmentCacheImplLowLevel::AreSegmentsSelected()
{
	if( 0 == numberOfSelectedSegments() ){
		return false;
	}

	return true;
}

bool OmSegmentCacheImplLowLevel::AreSegmentsEnabled()
{
	if( 0 == numberOfEnabledSegments() ){
		return false;
	}

	return true;
}

void OmSegmentCacheImplLowLevel::doSelectedSetInsert( const OmSegID segID,
						      const bool addToRecentList)
{
	mSelectedSet.insert( segID );
	if(addToRecentList) {
		addToRecentMap(segID);
	}
}

void OmSegmentCacheImplLowLevel::doSelectedSetRemove( const OmSegID segID)
{
	mSelectedSet.erase(segID);
	addToRecentMap(segID);
}

void OmSegmentCacheImplLowLevel::addToRecentMap( const OmSegID segID )
{
	segmentation_->GetSegmentLists()->TouchRecentList(segID);
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

OmSegID OmSegmentCacheImplLowLevel::GetSegmentationID()
{
	return segmentation_->GetID();
}

quint32 OmSegmentCacheImplLowLevel::getPageSize()
{
	return mSegments->getPageSize();
}

uint32_t OmSegmentCacheImplLowLevel::getMaxValue()
{
	return mMaxValue;
}

OmSegID OmSegmentCacheImplLowLevel::getNextValue()
{
	++mMaxValue;
	return mMaxValue;
}

void OmSegmentCacheImplLowLevel::clearCaches()
{
	OmCacheManager::TouchFresheness();
}

void OmSegmentCacheImplLowLevel::growGraphIfNeeded(OmSegment * newSeg)
{
	mSegmentGraph.growGraphIfNeeded(newSeg);
}

OmSegmentCache* OmSegmentCacheImplLowLevel::GetSegmentCache()
{
	return segmentation_->GetSegmentCache();
}
