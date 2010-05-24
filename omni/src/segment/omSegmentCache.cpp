#include "segment/omSegmentCache.h"
#include "utility/omHdf5Path.h"
#include "utility/omDataArchiveQT.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCacheImpl.h"
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

OmId OmSegmentCache::getSegmentationID()
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

OmSegment* OmSegmentCache::AddSegment(SEGMENT_DATA_TYPE value)
{
	QMutexLocker locker( &mMutex );
	return mImpl->AddSegment(value);
}

void OmSegmentCache::AddSegmentsFromChunk(const SegmentDataSet & data_values, 
					  const OmMipChunkCoord & mipCoord )
{
	QMutexLocker locker( &mMutex );
	mImpl->AddSegmentsFromChunk(data_values, mipCoord );
}

bool OmSegmentCache::isValueAlreadyMappedToSegment( SEGMENT_DATA_TYPE value )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isValueAlreadyMappedToSegment( value );
}

bool OmSegmentCache::IsSegmentValid(OmId seg)
{
	QMutexLocker locker( &mMutex );
	return mImpl->isValueAlreadyMappedToSegment(seg);
}

OmSegment* OmSegmentCache::GetSegmentFromValue(SEGMENT_DATA_TYPE value)
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetSegmentFromValue( value );
}

OmId OmSegmentCache::GetNumSegments()
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetNumSegments();
}

OmId OmSegmentCache::GetNumTopSegments()
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

OmIds & OmSegmentCache::GetSelectedSegmentIdsRef()
{
	QMutexLocker locker( &mMutex );
        return mImpl->GetSelectedSegmentIdsRef();
}

OmIds & OmSegmentCache::GetEnabledSegmentIdsRef()
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

bool OmSegmentCache::isSegmentEnabled( OmId segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentEnabled( segID );
}

bool OmSegmentCache::isSegmentSelected( OmId segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( segID );
}

bool OmSegmentCache::isSegmentSelected( OmSegment * seg )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( seg );
}

void OmSegmentCache::setSegmentEnabled( OmId segID, bool isEnabled )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentEnabled( segID, isEnabled );
}

void OmSegmentCache::setSegmentSelected( OmId segID, bool isSelected )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentSelected( segID, isSelected );
}

void OmSegmentCache::setSegmentName( OmId segID, QString name )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentName( segID, name );
}

QString OmSegmentCache::getSegmentName( OmId segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getSegmentName( segID );
}

void OmSegmentCache::setSegmentNote( OmId segID, QString note )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentNote( segID, note );
}

QString OmSegmentCache::getSegmentNote( OmId segID )
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

std::vector< OmSegment* > & OmSegmentCache::getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getSegmentListDirectCache( chunkCoord );
}

void OmSegmentCache::reloadDendrogram( const quint32 * dend, const float * dendValues, 
				       const int size, const float stopPoint )
{	
	QMutexLocker locker( &mMutex );
	return mImpl->reloadDendrogram( dend, dendValues, size, stopPoint );
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
