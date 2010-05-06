#include "segment/omSegmentCache.h"
#include "utility/omHdf5Path.h"
#include "utility/omDataArchiveQT.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCacheImpl.h"
#include <QMutexLocker>

OmSegmentCache::OmSegmentCache(OmSegmentation * segmentation)
{
	mImpl = new OmSegmentCacheImpl(segmentation, this);
	mSegmentation = segmentation;
}

OmSegmentCache::~OmSegmentCache()
{
	delete(mImpl);
}

void OmSegmentCache::turnBatchModeOn( const bool batchMode )
{
	QMutexLocker locker( &mMutex );
	mImpl->turnBatchModeOn( batchMode );
}

OmId OmSegmentCache::getSegmentationID()
{
	return mImpl->getSegmentationID();
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
	return mImpl->IsSegmentValid(seg);
}

OmSegment* OmSegmentCache::GetSegmentFromValue(SEGMENT_DATA_TYPE value)
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetSegmentFromValue( value );
}

OmSegment* OmSegmentCache::GetSegmentFromID(OmId segmentID)
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetSegmentFromID( segmentID );
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

SegmentDataSet OmSegmentCache::GetSelectedSegmentValues()
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetSelectedSegmentValues();
}

SegmentDataSet OmSegmentCache::GetEnabledSegmentValues()
{
	QMutexLocker locker( &mMutex );
	return mImpl->GetEnabledSegmentValues();
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

SegmentDataSet OmSegmentCache::getValues( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getValues( segment );
}

OmIds OmSegmentCache::getIDs( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	return mImpl->getIDs( segment );
}

OmSegment * OmSegmentCache::findRoot( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	return mImpl->findRoot( segment );
}

void OmSegmentCache::splitChildLowestThreshold( OmSegment * segment )
{
	QMutexLocker locker( &mMutex );
	mImpl->splitChildLowestThreshold( segment );
}
