#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omProjectData.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "volume/omSegmentation.h"

#include <QMutexLocker>
#include <vtkImageData.h>

OmSegmentCache::OmSegmentCache(OmSegmentation * segmentation)
	: mSegmentation(segmentation)
	, mImpl(new OmSegmentCacheImpl(segmentation, this))
{
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

// FIXME: this needs to get refactored into some sort of helper...
void OmSegmentCache::ExportDataFilter(vtkImageData * pImageData)
{
	QMutexLocker locker( &mMutex );

	//get data extent (varify it is a chunk)
	int extent[6];
	pImageData->GetExtent(extent);

	//get pointer to native scalar data
	assert(pImageData->GetScalarSize() == SEGMENT_DATA_BYTES_PER_SAMPLE);
	OmSegID * p_scalar_data = static_cast<OmSegID*>( pImageData->GetScalarPointer() );

	//for all voxels in the chunk
	int x, y, z;
	for (z = extent[0]; z <= extent[1]; z++) {
		for (y = extent[2]; y <= extent[3]; y++) {
			for (x = extent[4]; x <= extent[5]; x++) {

				//if non-null segment value
				if (NULL_SEGMENT_VALUE != *p_scalar_data) {
					*p_scalar_data = mImpl->findRootID( *p_scalar_data );
				}
				//adv to next scalar
				++p_scalar_data;
			}
		}
	}
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

bool OmSegmentCache::IsSegmentValid(OmSegID seg)
{
	QMutexLocker locker( &mMutex );
	return mImpl->isValueAlreadyMappedToSegment(seg);
}

OmSegment* OmSegmentCache::GetSegment(const OmSegID value)
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

OmSegIDsSet & OmSegmentCache::GetSelectedSegmentIds()
{
	QMutexLocker locker( &mMutex );
        return mImpl->GetSelectedSegmentIdsRef();
}

OmSegIDsSet & OmSegmentCache::GetEnabledSegmentIds()
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

bool OmSegmentCache::IsSegmentSelected( OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( segID );
}

bool OmSegmentCache::IsSegmentSelected( OmSegment * seg )
{
	QMutexLocker locker( &mMutex );
	return mImpl->isSegmentSelected( seg );
}

void OmSegmentCache::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentEnabled( segID, isEnabled );
}

void OmSegmentCache::setSegmentSelected( OmSegID segID,
					 const bool isSelected,
					 const bool addToRecentList)
{
	QMutexLocker locker( &mMutex );
	mImpl->setSegmentSelected( segID, isSelected, addToRecentList );
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

OmSegment * OmSegmentCache::findRoot( const OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->findRoot( segID );
}

OmSegID OmSegmentCache::findRootID( const OmSegID segID )
{
	QMutexLocker locker( &mMutex );
	return mImpl->findRootID( segID );
}

OmSegmentEdge OmSegmentCache::findClosestCommonEdge(OmSegment * seg1, OmSegment * seg2)
{
	QMutexLocker locker( &mMutex );
	return mImpl->findClosestCommonEdge(seg1, seg2);
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

quint64 OmSegmentCache::getSegmentListSize(OmSegIDRootType type)
{
	QMutexLocker locker( &mMutex );
        return mImpl->getSegmentListSize(type);
}

void OmSegmentCache::UpdateSegmentSelection( const OmSegIDsSet & idsToSelect,
					     const bool addToRecentList)
{
	QMutexLocker locker( &mMutex );
        return mImpl->UpdateSegmentSelection(idsToSelect, addToRecentList);
}

OmSegPtrListWithPage * OmSegmentCache::getRootLevelSegIDs( const unsigned int offset, const int numToGet, OmSegIDRootType type, OmSegID startSeg)
{
	QMutexLocker locker( &mMutex );
        return mImpl->getRootLevelSegIDs(offset, numToGet, type, startSeg);
}

void OmSegmentCache::setAsValidated(OmSegment * segment, const bool valid)
{
	QMutexLocker locker( &mMutex );
        return mImpl->setAsValidated(segment, valid);
}

OmSegmentEdge OmSegmentCache::JoinEdge( const OmSegmentEdge & e )
{
	QMutexLocker locker( &mMutex );
        return mImpl->JoinFromUserAction( e );
}

OmSegmentEdge OmSegmentCache::SplitEdge( const OmSegmentEdge & e )
{
	QMutexLocker locker( &mMutex );
        return mImpl->SplitEdgeUserAction( e );
}

void OmSegmentCache::refreshTree()
{
	QMutexLocker locker( &mMutex );
        return mImpl->refreshTree();
}

quint64 OmSegmentCache::getSizeRootAndAllChildren( OmSegment * segUnknownDepth )
{
	QMutexLocker locker( &mMutex );
        return mImpl->getSizeRootAndAllChildren(segUnknownDepth);
}

bool OmSegmentCache::AreSegmentsEnabled(){
	QMutexLocker locker( &mMutex );
	return mImpl->AreSegmentsEnabled();
}
