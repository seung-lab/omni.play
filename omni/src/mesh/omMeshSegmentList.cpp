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

bool OmMeshSegmentList::isSegmentListReadyInCache(OmMipChunkPtr p_chunk, 
						  OmSegment * rootSeg,
						  const OmMipChunkCoord & chunkCoord,
						  OmSegmentCache * mSegmentCache,
						  const OmId segmentationID)
{
	return Instance()->isSegmentListReadyInCache(p_chunk, 
						     rootSeg,
						     chunkCoord,
						     mSegmentCache,
						     segmentationID);
}

const OmSegPtrList & OmMeshSegmentList::getFromCache( const OmMipChunkCoord & c, 
						      const OmSegID rootSegID,
						      const OmId segmentationID )
{
	return Instance()->doGetFromCache( c, rootSegID, segmentationID );
}

void OmMeshSegmentList::addToCache( const OmMipChunkCoord & c, 
				    OmSegment * s, 
				    const OmSegPtrList & L,
				    const OmId sID)
{
	return Instance()->doAddToCache(c,s,L,sID);
}


bool OmMeshSegmentList::doIsSegmentListReadyInCache(OmMipChunkPtr p_chunk, 
						    OmSegment * rootSeg,
						    const OmMipChunkCoord & chunkCoord,
						    OmSegmentCache * mSegmentCache,
						    const OmId segmentationID)
{
	QMutexLocker lock(&mCacheLock);

	if(doIsCacheFetching(chunkCoord, rootSeg, segmentationID)){
		return false;
	}
	
	doClearFromCacheIfFreshnessInvalid(chunkCoord, rootSeg, segmentationID);
	
	if(!doCacheHasCoord( chunkCoord, rootSeg->getValue(), segmentationID )){
		doMakeSegmentListForCache( p_chunk, rootSeg, chunkCoord, mSegmentCache, segmentationID );
		return false;
	}

	return true;
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
					      const OmId segmentationID)
{
	mSegmentListCache[OmMeshSegListKey(segmentationID, rootSeg->getValue(), c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)] 
		= OmSegPtrListValid(true);
	debug("meshDrawer", "let cache know we are fetching\n");
}

bool OmMeshSegmentList::doIsCacheFetching( const OmMipChunkCoord & c,
				    OmSegment * rootSeg,
				    const OmId segmentationID )
{
	OmSegPtrListValid & spList = mSegmentListCache[OmMeshSegListKey(segmentationID, rootSeg->getValue(), c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)];
	return spList.isFetching;
}

void OmMeshSegmentList::doAddToCache( const OmMipChunkCoord & c,
			       OmSegment * rootSeg,
			       const OmSegPtrList & segmentsToDraw,
			       const OmId segmentationID )
{
	QMutexLocker lock(&mCacheLock);
	/*
	debug("meshDrawer", "added to cache\n");
	debug("meshDrawerVerbose", "added to cache: key is: %d, %d, %d, %d, %d, %d\n",
	      segmentationID,
	      rootSeg->getValue(),
	      c.Level,
	      c.Coordinate.x,
	      c.Coordinate.y,
	      c.Coordinate.z);
	*/
	mSegmentListCache[OmMeshSegListKey(segmentationID, rootSeg->getValue(), c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)] 
		= OmSegPtrListValid(segmentsToDraw, rootSeg->getFreshnessForMeshes() );
}

void OmMeshSegmentList::doClearFromCacheIfFreshnessInvalid( const OmMipChunkCoord & c,
						     OmSegment * rootSeg,
						     const OmId segmentationID)
{
	const unsigned int currentFreshness = rootSeg->getFreshnessForMeshes();

	OmSegPtrListValid & spList = mSegmentListCache[OmMeshSegListKey(segmentationID, rootSeg->getValue(), c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)];

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
				  const OmId segmentationID )
{
	OmSegPtrListValid & spList = mSegmentListCache[OmMeshSegListKey(segmentationID, rootSegID, c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)];
	return spList.isValid;
}

const OmSegPtrList & OmMeshSegmentList::doGetFromCache( const OmMipChunkCoord & c, const OmSegID rootSegID,
						 const OmId segmentationID )
{
	QMutexLocker lock(&mCacheLock);
	const OmSegPtrListValid & spList = mSegmentListCache[OmMeshSegListKey(segmentationID, rootSegID, c.Level, c.Coordinate.x, c.Coordinate.y, c.Coordinate.z)];
	return spList.list;
}
