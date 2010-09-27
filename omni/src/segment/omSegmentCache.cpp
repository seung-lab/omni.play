#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omProjectData.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "volume/omSegmentation.h"

OmSegmentCache::OmSegmentCache(OmSegmentation * segmentation)
	: mSegmentation(segmentation)
	, mImpl(new OmSegmentCacheImpl(segmentation))
{
}

OmSegmentCache::~OmSegmentCache()
{
}

OmSegID OmSegmentCache::getSegmentationID()
{
	return mSegmentation->GetId();
}

quint32 OmSegmentCache::getPageSize()
{
	return mImpl->getPageSize();
}

void OmSegmentCache::turnBatchModeOn( const bool batchMode )
{
	zi::guard g(mutex_);
	mImpl->turnBatchModeOn( batchMode );
}
OmSegment* OmSegmentCache::AddSegment()
{
	zi::guard g(mutex_);
	return mImpl->AddSegment();
}

OmSegment* OmSegmentCache::AddSegment(OmSegID value)
{
	zi::guard g(mutex_);
	return mImpl->AddSegment(value);
}

OmSegment* OmSegmentCache::GetOrAddSegment(const OmSegID val)
{
	zi::guard g(mutex_);
	return mImpl->GetOrAddSegment(val);
}

bool OmSegmentCache::IsSegmentValid(OmSegID seg)
{
	zi::guard g(mutex_);
	return mImpl->isValueAlreadyMappedToSegment(seg);
}

OmSegment* OmSegmentCache::GetSegment(const OmSegID value)
{
	zi::guard g(mutex_);
	return mImpl->GetSegmentFromValue( value );
}

OmSegID OmSegmentCache::GetNumSegments()
{
	zi::guard g(mutex_);
	return mImpl->GetNumSegments();
}

OmSegID OmSegmentCache::GetNumTopSegments()
{
	zi::guard g(mutex_);
	return mImpl->GetNumTopSegments();
}

bool OmSegmentCache::AreSegmentsSelected()
{
	zi::guard g(mutex_);
        return mImpl->AreSegmentsSelected();
}

quint32 OmSegmentCache::numberOfSelectedSegments()
{
	zi::guard g(mutex_);
	return mImpl->numberOfSelectedSegments();
}

OmSegIDsSet & OmSegmentCache::GetSelectedSegmentIds()
{
	zi::guard g(mutex_);
        return mImpl->GetSelectedSegmentIdsRef();
}

OmSegIDsSet & OmSegmentCache::GetEnabledSegmentIds()
{
	zi::guard g(mutex_);
        return mImpl->GetEnabledSegmentIdsRef();
}

void OmSegmentCache::SetAllEnabled(bool enabled)
{
	zi::guard g(mutex_);
	mImpl->SetAllEnabled(enabled);
}

void OmSegmentCache::SetAllSelected(bool selected)
{
	zi::guard g(mutex_);
	mImpl->SetAllSelected(selected);
}

bool OmSegmentCache::isSegmentEnabled( OmSegID segID )
{
	zi::guard g(mutex_);
	return mImpl->isSegmentEnabled( segID );
}

bool OmSegmentCache::IsSegmentSelected( OmSegID segID )
{
	zi::guard g(mutex_);
	return mImpl->isSegmentSelected( segID );
}

bool OmSegmentCache::IsSegmentSelected( OmSegment * seg )
{
	zi::guard g(mutex_);
	return mImpl->isSegmentSelected( seg );
}

void OmSegmentCache::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	zi::guard g(mutex_);
	mImpl->setSegmentEnabled( segID, isEnabled );
}

void OmSegmentCache::setSegmentSelected( OmSegID segID,
					 const bool isSelected,
					 const bool addToRecentList)
{
	zi::guard g(mutex_);
	mImpl->setSegmentSelected( segID, isSelected, addToRecentList );
}

void OmSegmentCache::setSegmentName( OmSegID segID, QString name )
{
	zi::guard g(mutex_);
	mImpl->setSegmentName(segID, name);
}

QString OmSegmentCache::getSegmentName( OmSegID segID )
{
	zi::guard g(mutex_);
	return mImpl->getSegmentName( segID );
}

void OmSegmentCache::setSegmentNote( OmSegID segID, QString note )
{
	zi::guard g(mutex_);
	mImpl->setSegmentNote( segID, note );
}

QString OmSegmentCache::getSegmentNote( OmSegID segID )
{
	zi::guard g(mutex_);
	return mImpl->getSegmentNote( segID );
}

void OmSegmentCache::addToDirtySegmentList( OmSegment* seg)
{
	zi::guard g(mutex_);
	mImpl->addToDirtySegmentList( seg);
}

void OmSegmentCache::flushDirtySegments()
{
	zi::guard g(mutex_);
	mImpl->flushDirtySegments();
}

OmSegment* OmSegmentCache::findRoot(OmSegment * segment)
{
	zi::guard g(mutex_);
	return mImpl->findRoot( segment );
}

OmSegment* OmSegmentCache::findRoot(const OmSegID segID)
{
	zi::guard g(mutex_);
	return mImpl->findRoot( segID );
}

OmSegID OmSegmentCache::findRootID(const OmSegID segID)
{
	zi::guard g(mutex_);
	return mImpl->findRootID( segID );
}

OmSegmentEdge OmSegmentCache::findClosestCommonEdge(OmSegment * seg1,
						    OmSegment * seg2)
{
	zi::guard g(mutex_);
	return mImpl->findClosestCommonEdge(seg1, seg2);
}

void OmSegmentCache::JoinTheseSegments( const OmSegIDsSet & segmentList)
{
	zi::guard g(mutex_);
	mImpl->JoinTheseSegments(segmentList);
}

void OmSegmentCache::UnJoinTheseSegments( const OmSegIDsSet & segmentList)
{
	zi::guard g(mutex_);
	mImpl->UnJoinTheseSegments( segmentList);
}

quint32 OmSegmentCache::getMaxValue()
{
	zi::guard g(mutex_);
        return mImpl->getMaxValue();
}

quint64 OmSegmentCache::getSegmentListSize(OmSegIDRootType type)
{
	zi::guard g(mutex_);
        return mImpl->getSegmentListSize(type);
}

void OmSegmentCache::UpdateSegmentSelection( const OmSegIDsSet & idsToSelect,
					     const bool addToRecentList)
{
	zi::guard g(mutex_);
        return mImpl->UpdateSegmentSelection(idsToSelect, addToRecentList);
}

OmSegPtrListWithPage * OmSegmentCache::getRootLevelSegIDs(const uint32_t offset,
							  const int numToGet,
							  const OmSegIDRootType type,
							  const OmSegID startSeg)
{
	zi::guard g(mutex_);
        return mImpl->getRootLevelSegIDs(offset, numToGet, type, startSeg);
}

void OmSegmentCache::setAsValidated(OmSegment * segment, const bool valid)
{
	zi::guard g(mutex_);
        return mImpl->setAsValidated(segment, valid);
}

std::pair<bool, OmSegmentEdge> OmSegmentCache::JoinEdge( const OmSegmentEdge & e )
{
	zi::guard g(mutex_);
        return mImpl->JoinFromUserAction( e );
}

OmSegmentEdge OmSegmentCache::SplitEdge( const OmSegmentEdge & e )
{
	zi::guard g(mutex_);
        return mImpl->SplitEdgeUserAction( e );
}

void OmSegmentCache::refreshTree()
{
	zi::guard g(mutex_);
        return mImpl->refreshTree();
}

quint64 OmSegmentCache::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	zi::guard g(mutex_);
        return mImpl->getSizeRootAndAllChildren(segUnknownDepth);
}

bool OmSegmentCache::AreSegmentsEnabled(){
	zi::guard g(mutex_);
	return mImpl->AreSegmentsEnabled();
}
