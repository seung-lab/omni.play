#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omProjectData.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "volume/omSegmentation.h"

#include <QMutexLocker>

OmSegmentCache::OmSegmentCache(OmSegmentation * segmentation)
	: mSegmentation(segmentation)
{
	mImpl = new OmSegmentCacheImpl(segmentation, this); 
}

OmSegmentCache::~OmSegmentCache()
{
	delete mImpl;
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
	QMutexLocker locker( &mMutex );
	mImpl->turnBatchModeOn( batchMode );
}
OmSegment* OmSegmentCache::AddSegment()
{
	QMutexLocker locker( &mMutex );
	return mImpl->AddSegment();
}

OmSegment* OmSegmentCache::AddSegment(OmSegID value)
{
	QMutexLocker locker( &mMutex );
	return mImpl->AddSegment(value);
}

void OmSegmentCache::AddSegmentsFromChunk(const OmSegIDsSet & data_values, 
					  const OmMipChunkCoord & mipCoord,
					  boost::unordered_map< OmSegID, unsigned int> * sizes )
{
	QMutexLocker locker( &mMutex );
	mImpl->AddSegmentsFromChunk(data_values, mipCoord, sizes );
}

bool OmSegmentCache::isValueAlreadyMappedToSegment( OmSegID value )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isValueAlreadyMappedToSegment( value );
}

bool OmSegmentCache::IsSegmentValid(OmSegID seg)
{
	QMutexLocker locker( &mMutex );
	return mImpl->isValueAlreadyMappedToSegment(seg);
}

OmSegment* OmSegmentCache::GetSegmentFromValue(OmSegID value)
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetSegmentFromValue( value );
}

OmSegID OmSegmentCache::GetNumSegments()
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetNumSegments();
}

OmSegID OmSegmentCache::GetNumTopSegments()
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetNumTopSegments();
}

bool OmSegmentCache::AreSegmentsSelected()
{
	QMutexLocker locker( &mMutex );
        return mImpl->AreSegmentsSelected();
}

quint32 OmSegmentCache::numberOfSelectedSegments()
{
	QMutexLocker locker( &mMutex );
	return mImpl->numberOfSelectedSegments();
}

OmSegIDsSet & OmSegmentCache::GetSelectedSegmentIdsRef()
{
	QMutexLocker locker( &mMutex );
        return mImpl->GetSelectedSegmentIdsRef();
}

OmSegIDsSet & OmSegmentCache::GetEnabledSegmentIdsRef()
{
	QMutexLocker locker( &mMutex );
        return mImpl->GetEnabledSegmentIdsRef();
}

void OmSegmentCache::SetAllEnabled(bool enabled)
{
	QMutexLocker locker( &mMutex );
	mImpl->SetAllEnabled(enabled);
}

void OmSegmentCache::SetAllSelected(bool selected)
{
	QMutexLocker locker( &mMutex );
	mImpl->SetAllSelected(selected);
}

bool OmSegmentCache::isSegmentEnabled( OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentEnabled( segID );
}

bool OmSegmentCache::isSegmentSelected( OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( segID );
}

bool OmSegmentCache::isSegmentSelected( OmSegment * seg )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( seg );
}

void OmSegmentCache::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentEnabled( segID, isEnabled );
}

void OmSegmentCache::setSegmentSelected( OmSegID segID, bool isSelected )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentSelected( segID, isSelected );
}

void OmSegmentCache::setSegmentName( OmSegID segID, QString name )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentName(segID, name);
}

QString OmSegmentCache::getSegmentName( OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getSegmentName( segID );
}

void OmSegmentCache::setSegmentNote( OmSegID segID, QString note )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentNote( segID, note );
}

QString OmSegmentCache::getSegmentNote( OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getSegmentNote( segID );
}

void OmSegmentCache::addToDirtySegmentList( OmSegment* seg)
{
	QMutexLocker locker( &mMutex );
	mImpl->addToDirtySegmentList( seg);
}

void OmSegmentCache::flushDirtySegments()
{
	QMutexLocker locker( &mMutex );
	mImpl->flushDirtySegments();
}

OmSegment * OmSegmentCache::findRoot( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	return mImpl->findRoot( segment );
}

OmSegID OmSegmentCache::findRootID( const OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->findRootID( segID );
}

OmSegID OmSegmentCache::findRootID_noLock( const OmSegID segID ) 
{
	return mImpl->findRootID( segID );
}

OmSegmentEdge * OmSegmentCache::splitTwoChildren(OmSegment * seg1, OmSegment * seg2)
{
	QMutexLocker locker( &mMutex );
	return mImpl->splitTwoChildren(seg1, seg2);
}

void OmSegmentCache::resetGlobalThreshold( const float stopPoint )
{	
	QMutexLocker locker( &mMutex );
	return mImpl->resetGlobalThreshold( stopPoint );
}

void OmSegmentCache::JoinTheseSegments( const OmSegIDsSet & segmentList)
{
	QMutexLocker locker( &mMutex );
	mImpl->JoinTheseSegments(segmentList);
}

void OmSegmentCache::UnJoinTheseSegments( const OmSegIDsSet & segmentList)
{
	QMutexLocker locker( &mMutex );
	mImpl->UnJoinTheseSegments( segmentList);
}

quint32 OmSegmentCache::getMaxValue()
{
	QMutexLocker locker( &mMutex );
        return mImpl->getMaxValue();
}

void OmSegmentCache::UpdateSegmentSelection( const OmSegIDsSet & idsToSelect)
{
	QMutexLocker locker( &mMutex );
        return mImpl->UpdateSegmentSelection(idsToSelect);
}

OmSegPtrList * OmSegmentCache::getRootLevelSegIDs( const unsigned int offset, const int numToGet )
{
	QMutexLocker locker( &mMutex );
        return mImpl->getRootLevelSegIDs(offset, numToGet);
}

void OmSegmentCache::setAsValidated( const OmSegIDsList & segmentsToGroup )
{
	QMutexLocker locker( &mMutex );
        return mImpl->setAsValidated( segmentsToGroup );
}

void OmSegmentCache::unsetAsValidated( const OmSegIDsList & segmentsToGroup )
{
	QMutexLocker locker( &mMutex );
        return mImpl->unsetAsValidated( segmentsToGroup );
}

OmSegmentEdge * OmSegmentCache::JoinEdge( OmSegmentEdge * e )
{
	QMutexLocker locker( &mMutex );
        return mImpl->Join( e );
}
