#include "mesh/omMeshSegmentList.h"
#include "mesh/omMeshSegmentListThread.h"
#include "common/omDebug.h"

OmMeshSegmentList* OmMeshSegmentList::mspInstance = NULL;

static const int MAX_THREADS = 3;

OmMeshSegmentList::OmMeshSegmentList()
{
	mThreadPool.start(MAX_THREADS);
}

OmMeshSegmentList::~OmMeshSegmentList()
{
	mThreadPool.stop();
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

void OmMeshSegmentList::addToCache( const OmMipChunkCoord & c,
				    OmSegment * rootSeg,
				    const OmSegPtrList & segmentsToDraw,
				    const OmId segmentationID )
{
  zi::Guard g(Instance()->mutex);

  Instance()->mSegmentListCache[makeKey(segmentationID, rootSeg->value, c)]
    = OmSegPtrListValid(segmentsToDraw, rootSeg->getFreshnessForMeshes() );
}

std::pair<bool, OmSegPtrList>
OmMeshSegmentList::doGetFromCacheIfReady(OmMipChunkPtr p_chunk,
				       OmSegment * rootSeg,
				       const OmMipChunkCoord & chunkCoord,
				       boost::shared_ptr<OmSegmentCache> mSegmentCache,
				       const OmId segmentationID)
{
  zi::Guard g(mutex);

  OmSegPtrListValid & spList =
    mSegmentListCache[makeKey(segmentationID,
			      rootSeg->value,
			      chunkCoord)];

  if(spList.isFetching){ // coord already in queue to be fetched
    return std::make_pair(false, OmSegPtrList());
  }

  // remove from cache if freshness is too old
  const unsigned int currentFreshness = rootSeg->getFreshnessForMeshes();
  if(spList.isValid && currentFreshness != spList.freshness){
    spList.list.clear();
    spList.isValid = false;
  }

  if(!spList.isValid){ // add coord to list to be fetched
    spList = OmSegPtrListValid(true);

    boost::shared_ptr<OmMeshSegmentListThread>
      task(new OmMeshSegmentListThread(p_chunk,
				       rootSeg,
				       chunkCoord,
				       mSegmentCache,
				       segmentationID));
    mThreadPool.addTaskFront(task);
    return std::make_pair(false, OmSegPtrList());
  }

  // coord was valid
  return make_pair(true, spList.list);
}
