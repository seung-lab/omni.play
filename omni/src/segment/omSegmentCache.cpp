#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omProjectData.h"
#include "utility/omDataArchiveQT.h"
#include "utility/omHdf5Path.h"
#include "utility/omHdf5Path.h"
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

void OmSegmentCache::AddSegmentsFromChunk(const OmSegIDs & data_values, 
					  const OmMipChunkCoord & mipCoord )
{
	QMutexLocker locker( &mMutex );
	mImpl->AddSegmentsFromChunk(data_values, mipCoord );
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

OmSegIDs & OmSegmentCache::GetSelectedSegmentIdsRef()
{
	QMutexLocker locker( &mMutex );
        return mImpl->GetSelectedSegmentIdsRef();
}

OmSegIDs & OmSegmentCache::GetEnabledSegmentIdsRef()
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
	mImpl->setSegmentName( segID, name );
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

void OmSegmentCache::splitTwoChildren(OmSegment * seg1, OmSegment * seg2)
{
	QMutexLocker locker( &mMutex );
	mImpl->splitTwoChildren(seg1, seg2);
}


void OmSegmentCache::splitChildLowestThreshold( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	mImpl->splitChildLowestThreshold( segment );
}

void OmSegmentCache::setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
						std::vector< OmSegment* > & segmentsToDraw )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentListDirectCache( chunkCoord, segmentsToDraw );
}

bool OmSegmentCache::segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord )
{
	QMutexLocker locker( &mMutex );
	return mImpl->segmentListDirectCacheHasCoord( chunkCoord );
}

const OmSegPtrs & OmSegmentCache::getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getSegmentListDirectCache( chunkCoord );
}

void OmSegmentCache::resetGlobalThreshold( const float stopPoint )
{	
	QMutexLocker locker( &mMutex );
	return mImpl->resetGlobalThreshold( stopPoint );
}

void OmSegmentCache::JoinAllSegmentsInSelectedList()
{
	QMutexLocker locker( &mMutex );
	return mImpl->JoinAllSegmentsInSelectedList();
}

quint32 OmSegmentCache::getMaxValue()
{
	QMutexLocker locker( &mMutex );
        return mImpl->getMaxValue();
}

void OmSegmentCache::UpdateSegmentSelection( const OmSegIDs & ids, const bool areSelected )
{
	QMutexLocker locker( &mMutex );
        return mImpl->UpdateSegmentSelection( ids, areSelected );
}
