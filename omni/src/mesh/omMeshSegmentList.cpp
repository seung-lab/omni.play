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

void OmMeshSegmentList::addToCache(OmMipChunkPtr chunk, OmSegment* rootSeg,
				    const OmSegPtrList & segmentsToDraw)
{
  zi::Guard g(Instance()->mutex);

  Instance()->mSegmentListCache[makeKey(chunk, rootSeg)]
    = OmSegPtrListValid(segmentsToDraw, rootSeg->getFreshnessForMeshes() );
}

std::pair<bool, OmSegPtrList>
OmMeshSegmentList::doGetFromCacheIfReady(OmMipChunkPtr chunk, OmSegment* rootSeg)
{
  zi::Guard g(mutex);

  OmSegPtrListValid& spList = mSegmentListCache[makeKey(chunk, rootSeg)];

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
	    task(new OmMeshSegmentListThread(chunk, rootSeg));

    mThreadPool.addTaskFront(task);
    return std::make_pair(false, OmSegPtrList());
  }

  // coord was valid
  return make_pair(true, spList.list);
}
