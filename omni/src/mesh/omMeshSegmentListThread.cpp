#include "mesh/omMeshSegmentListThread.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"
#include "mesh/omMeshSegmentList.h"
#include "system/omEvents.h"

OmMeshSegmentListThread::OmMeshSegmentListThread(OmMipChunkPtr p_chunk,
												 OmSegment * rootSeg)
	: mChunk(p_chunk)
	, mRootSeg(rootSeg)
{
}

void OmMeshSegmentListThread::run()
{
	const OmSegIDsSet & chunkValues =  mChunk->GetDirectDataValues();
	OmSegmentIterator segIter(mRootSeg->GetSegmentCache());
	segIter.iterOverSegmentID(mRootSeg->value());
	OmSegment* seg = segIter.getNextSegment();

	OmSegPtrList segmentsToDraw;

	while( NULL != seg ){
		if(0 != chunkValues.count(seg->value())){
			segmentsToDraw.push_back(seg);
		}

		seg = segIter.getNextSegment();
	}

	OmMeshSegmentList::addToCache(mChunk, mRootSeg, segmentsToDraw);
	OmEvents::Redraw3d();
	//	printf("done..(%u)\n", mRootSeg->value);
}
