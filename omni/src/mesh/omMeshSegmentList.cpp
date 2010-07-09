#include "mesh/omMeshSegmentList.h"
#include "mesh/omMeshSegmentListThread.h"
#include "common/omDebug.h"
#include "volume/omMipChunk.h"
#include <QThreadPool>

OmMeshSegmentList* OmMeshSegmentList::mspInstance = NULL;

OmMeshSegmentList::OmMeshSegmentList()
{
}

OmMeshSegmentList::~OmMeshSegmentList()
{
}

OmMeshSegmentList* OmMeshSegmentList::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmMeshSegmentList();
	}
	return mspInstance;
}


void OmMeshSegmentList::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}
	mspInstance = NULL;
}

void OmMeshSegmentList::makeSegmentListForCache(OmMipChunkPtr p_chunk, 
					   OmSegment * rootSeg,
					   const OmMipChunkCoord & chunkCoord,
					   OmSegmentCache * mSegmentCache,
					   const OmId segmentationID)
{
	return Instance()->doMakeSegmentListForCache(p_chunk, rootSeg, chunkCoord, mSegmentCache, segmentationID);
}

void OmMeshSegmentList::letCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					      OmSegment * rootSeg,
					      const OmId mSegmentationID)
{
	return Instance()->doLetCacheKnowWeAreFetching( c, rootSeg, mSegmentationID);
}

bool OmMeshSegmentList::isCacheFetching( const OmMipChunkCoord & c,
				    OmSegment * rootSeg,
				    const OmId mSegmentationID )
{
	return Instance()->doIsCacheFetching( c, rootSeg, mSegmentationID );
}

void OmMeshSegmentList::addToCache( const OmMipChunkCoord & c,
			       OmSegment * rootSeg,
			       const OmSegPtrList & segmentsToDraw,
			       const OmId mSegmentationID )
{
	return Instance()->doAddToCache( c, rootSeg, segmentsToDraw, mSegmentationID );
}

void OmMeshSegmentList::clearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						     OmSegment * rootSeg,
						     const OmId mSegmentationID)
{
	return Instance()->doClearFromCacheIfFreshnessInvalid( c, rootSeg, mSegmentationID);
}

bool OmMeshSegmentList::cacheHasCoord( const OmMipChunkCoord & c, const OmSegID rootSegID,
				  const OmId mSegmentationID )
{
	return Instance()->doCacheHasCoord( c, rootSegID, mSegmentationID );
}

const OmSegPtrList & OmMeshSegmentList::getFromCache( const OmMipChunkCoord & c, 
						      const OmSegID rootSegID,
						      const OmId mSegmentationID )
{
	return Instance()->doGetFromCache( c, rootSegID, mSegmentationID );
}



void OmMeshSegmentList::doMakeSegmentListForCache(OmMipChunkPtr p_chunk, 
					   OmSegment * rootSeg,
					   const OmMipChunkCoord & chunkCoord,
					   OmSegmentCache * mSegmentCache,
					   const OmId segmentationID)
{
	doLetCacheKnowWeAreFetching( chunkCoord, rootSeg, segmentationID );

	OmMeshSegmentListThread* thread = new OmMeshSegmentListThread(p_chunk, 
								      rootSeg,
								      chunkCoord,
								      mSegmentCache,
								      segmentationID);
	QThreadPool::globalInstance()->start(thread);
}

void OmMeshSegmentList::doLetCacheKnowWeAreFetching( const OmMipChunkCoord & c,
					      OmSegment * rootSeg,
					      const OmId mSegmentationID)
{
	QMutexLocker lock(&mCacheLock);
	mSegmentListCache[ mSegmentationID ][rootSeg->getValue()][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z] = OmSegPtrListValid(true);
	debug("meshDrawer", "let cache know we are fetching\n");
}

bool OmMeshSegmentList::doIsCacheFetching( const OmMipChunkCoord & c,
				    OmSegment * rootSeg,
				    const OmId mSegmentationID )
{
	QMutexLocker lock(&mCacheLock);
	OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][rootSeg->getValue()][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.isFetching;
}

void OmMeshSegmentList::doAddToCache( const OmMipChunkCoord & c,
			       OmSegment * rootSeg,
			       const OmSegPtrList & segmentsToDraw,
			       const OmId mSegmentationID )
{
	QMutexLocker lock(&mCacheLock);
	/*
	debug("meshDrawer", "added to cache\n");
	debug("meshDrawerVerbose", "added to cache: key is: %d, %d, %d, %d, %d, %d\n",
	      mSegmentationID,
	      rootSeg->getValue(),
	      c.Level,
	      c.Coordinate.x,
	      c.Coordinate.y,
	      c.Coordinate.z);
	*/
	mSegmentListCache[ mSegmentationID ][rootSeg->getValue()][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z] = OmSegPtrListValid(segmentsToDraw, rootSeg->getFreshnessForMeshes() );
}

void OmMeshSegmentList::doClearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						     OmSegment * rootSeg,
						     const OmId mSegmentationID)
{
	QMutexLocker lock(&mCacheLock);
	const unsigned int currentFreshness = rootSeg->getFreshnessForMeshes();

	OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][rootSeg->getValue()][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];

	if(!spList.isValid){
		return;
	}
	
	if(currentFreshness != spList.freshness &&
	   !spList.isFetching){
		spList.list.clear();
		spList.isValid = false;
		debug("meshDrawer", "%s: currentFreshness is %i, cache is %i\n", __FUNCTION__,
		      currentFreshness, spList.freshness);
	}
}

bool OmMeshSegmentList::doCacheHasCoord( const OmMipChunkCoord & c, const OmSegID rootSegID,
				  const OmId mSegmentationID )
{
	QMutexLocker lock(&mCacheLock);
	OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][rootSegID][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.isValid;
}

const OmSegPtrList & OmMeshSegmentList::doGetFromCache( const OmMipChunkCoord & c, const OmSegID rootSegID,
						 const OmId mSegmentationID )
{
	QMutexLocker lock(&mCacheLock);
	const OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][rootSegID][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.list;
}
