#include "mesh/omMeshDrawer.h"
#include "mesh/omMeshSegmentListThread.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"
#include "mesh/omMeshSegmentList.h"
#include "system/omEvents.h"

OmMeshSegmentListThread::OmMeshSegmentListThread( OmMipChunkPtr p_chunk,
						  OmSegment * rootSeg,
						  const OmMipChunkCoord & chunkCoord,
						  boost::shared_ptr<OmSegmentCache> segmentCache,
						  const OmId segmentationID)
	: mChunk(p_chunk)
	, mRootSeg(rootSeg)
	, mChunkCoord(chunkCoord)
	, mSegmentCache(segmentCache)
	, mSegmentationID(segmentationID)

{
}

void OmMeshSegmentListThread::run()
{
	const OmSegIDsSet & chunkValues =  mChunk->GetDirectDataValues();
	OmSegmentIterator segIter(mSegmentCache);
	segIter.iterOverSegmentID(mRootSeg->value);
	OmSegment * seg = segIter.getNextSegment();

	OmSegPtrList segmentsToDraw;

	while( NULL != seg ){
		const OmSegID val = seg->value;
		if( chunkValues.contains( val ) ){
			segmentsToDraw.push_back(seg);
		}

		seg = segIter.getNextSegment();
	}

	OmMeshSegmentList::addToCache( mChunkCoord, mRootSeg, segmentsToDraw, mSegmentationID );
	OmEvents::Redraw3d();
	//	printf("done..(%u)\n", mRootSeg->value);
}
