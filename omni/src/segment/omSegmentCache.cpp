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
	zi::Guard g(mMutex);
	mImpl->turnBatchModeOn( batchMode );
}
OmSegment* OmSegmentCache::AddSegment()
{
	zi::Guard g(mMutex);
	return mImpl->AddSegment();
}

OmSegment* OmSegmentCache::AddSegment(OmSegID value)
{
	zi::Guard g(mMutex);
	return mImpl->AddSegment(value);
}

OmSegment* OmSegmentCache::GetOrAddSegment(const OmSegID val)
{
	zi::Guard g(mMutex);
	return mImpl->GetOrAddSegment(val);
}

bool OmSegmentCache::IsSegmentValid(OmSegID seg)
{
	zi::Guard g(mMutex);
	return mImpl->isValueAlreadyMappedToSegment(seg);
}

OmSegment* OmSegmentCache::GetSegment(const OmSegID value)
{
	zi::Guard g(mMutex);
	return mImpl->GetSegmentFromValue( value );
}

OmSegID OmSegmentCache::GetNumSegments()
{
	zi::Guard g(mMutex);
	return mImpl->GetNumSegments();
}

OmSegID OmSegmentCache::GetNumTopSegments()
{
	zi::Guard g(mMutex);
	return mImpl->GetNumTopSegments();
}

bool OmSegmentCache::AreSegmentsSelected()
{
	zi::Guard g(mMutex);
        return mImpl->AreSegmentsSelected();
}

quint32 OmSegmentCache::numberOfSelectedSegments()
{
	zi::Guard g(mMutex);
	return mImpl->numberOfSelectedSegments();
}

OmSegIDsSet & OmSegmentCache::GetSelectedSegmentIds()
{
	zi::Guard g(mMutex);
        return mImpl->GetSelectedSegmentIdsRef();
}

OmSegIDsSet & OmSegmentCache::GetEnabledSegmentIds()
{
	zi::Guard g(mMutex);
        return mImpl->GetEnabledSegmentIdsRef();
}

void OmSegmentCache::SetAllEnabled(bool enabled)
{
	zi::Guard g(mMutex);
	mImpl->SetAllEnabled(enabled);
}

void OmSegmentCache::SetAllSelected(bool selected)
{
	zi::Guard g(mMutex);
	mImpl->SetAllSelected(selected);
}

bool OmSegmentCache::isSegmentEnabled( OmSegID segID )
{
	zi::Guard g(mMutex);
	return mImpl->isSegmentEnabled( segID );
}

bool OmSegmentCache::IsSegmentSelected( OmSegID segID )
{
	zi::Guard g(mMutex);
	return mImpl->isSegmentSelected( segID );
}

bool OmSegmentCache::IsSegmentSelected( OmSegment * seg )
{
	zi::Guard g(mMutex);
	return mImpl->isSegmentSelected( seg );
}

void OmSegmentCache::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	zi::Guard g(mMutex);
	mImpl->setSegmentEnabled( segID, isEnabled );
}

void OmSegmentCache::setSegmentSelected( OmSegID segID,
					 const bool isSelected,
					 const bool addToRecentList)
{
	zi::Guard g(mMutex);
	mImpl->setSegmentSelected( segID, isSelected, addToRecentList );
}

void OmSegmentCache::setSegmentName( OmSegID segID, QString name )
{
	zi::Guard g(mMutex);
	mImpl->setSegmentName(segID, name);
}

QString OmSegmentCache::getSegmentName( OmSegID segID )
{
	zi::Guard g(mMutex);
	return mImpl->getSegmentName( segID );
}

void OmSegmentCache::setSegmentNote( OmSegID segID, QString note )
{
	zi::Guard g(mMutex);
	mImpl->setSegmentNote( segID, note );
}

QString OmSegmentCache::getSegmentNote( OmSegID segID )
{
	zi::Guard g(mMutex);
	return mImpl->getSegmentNote( segID );
}

void OmSegmentCache::addToDirtySegmentList( OmSegment* seg)
{
	zi::Guard g(mMutex);
	mImpl->addToDirtySegmentList( seg);
}

void OmSegmentCache::flushDirtySegments()
{
	zi::Guard g(mMutex);
	mImpl->flushDirtySegments();
}

OmSegment* OmSegmentCache::findRoot(OmSegment * segment)
{
	zi::Guard g(mMutex);
	return mImpl->findRoot( segment );
}

OmSegment* OmSegmentCache::findRoot(const OmSegID segID)
{
	zi::Guard g(mMutex);
	return mImpl->findRoot( segID );
}

OmSegID OmSegmentCache::findRootID(const OmSegID segID)
{
	zi::Guard g(mMutex);
	return mImpl->findRootID( segID );
}

OmSegmentEdge OmSegmentCache::findClosestCommonEdge(OmSegment * seg1,
						    OmSegment * seg2)
{
	zi::Guard g(mMutex);
	return mImpl->findClosestCommonEdge(seg1, seg2);
}

void OmSegmentCache::JoinTheseSegments( const OmSegIDsSet & segmentList)
{
	zi::Guard g(mMutex);
	mImpl->JoinTheseSegments(segmentList);
}

void OmSegmentCache::UnJoinTheseSegments( const OmSegIDsSet & segmentList)
{
	zi::Guard g(mMutex);
	mImpl->UnJoinTheseSegments( segmentList);
}

quint32 OmSegmentCache::getMaxValue()
{
	zi::Guard g(mMutex);
        return mImpl->getMaxValue();
}

quint64 OmSegmentCache::getSegmentListSize(OmSegIDRootType type)
{
	zi::Guard g(mMutex);
        return mImpl->getSegmentListSize(type);
}

void OmSegmentCache::UpdateSegmentSelection( const OmSegIDsSet & idsToSelect,
					     const bool addToRecentList)
{
	zi::Guard g(mMutex);
        return mImpl->UpdateSegmentSelection(idsToSelect, addToRecentList);
}

OmSegPtrListWithPage * OmSegmentCache::getRootLevelSegIDs(const uint32_t offset,
							  const int numToGet,
							  const OmSegIDRootType type,
							  const OmSegID startSeg)
{
	zi::Guard g(mMutex);
        return mImpl->getRootLevelSegIDs(offset, numToGet, type, startSeg);
}

void OmSegmentCache::setAsValidated(OmSegment * segment, const bool valid)
{
	zi::Guard g(mMutex);
        return mImpl->setAsValidated(segment, valid);
}

std::pair<bool, OmSegmentEdge> OmSegmentCache::JoinEdge( const OmSegmentEdge & e )
{
	zi::Guard g(mMutex);
        return mImpl->JoinFromUserAction( e );
}

OmSegmentEdge OmSegmentCache::SplitEdge( const OmSegmentEdge & e )
{
	zi::Guard g(mMutex);
        return mImpl->SplitEdgeUserAction( e );
}

void OmSegmentCache::refreshTree()
{
	zi::Guard g(mMutex);
        return mImpl->refreshTree();
}

quint64 OmSegmentCache::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	zi::Guard g(mMutex);
        return mImpl->getSizeRootAndAllChildren(segUnknownDepth);
}

bool OmSegmentCache::AreSegmentsEnabled(){
	zi::Guard g(mMutex);
	return mImpl->AreSegmentsEnabled();
}
