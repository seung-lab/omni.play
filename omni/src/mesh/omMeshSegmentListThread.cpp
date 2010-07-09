#include "mesh/omMeshDrawer.h"
#include "mesh/omMeshSegmentListThread.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"

OmMeshSegmentListThread::OmMeshSegmentListThread( OmMipChunkPtr p_chunk, 
						  OmSegment * rootSeg,
						  const OmMipChunkCoord & chunkCoord,
						  OmSegmentCache * segmentCache,
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
	OmMeshDrawer::letCacheKnowWeAreFetching( mChunkCoord, mRootSeg, mSegmentationID );

	const OmSegIDsSet & chunkValues =  mChunk->GetDirectDataValues();
	OmSegmentIterator segIter(mSegmentCache);
	segIter.iterOverSegmentID(mRootSeg->getValue());
	OmSegment * seg = segIter.getNextSegment();
	OmSegID val;

	OmSegPtrList segmentsToDraw;

	while( NULL != seg ){
		val = seg->getValue();
		if( chunkValues.contains( val ) ){
			segmentsToDraw.push_back(seg);
		}
				
		seg = segIter.getNextSegment();
	}

	OmMeshDrawer::addToCache( mChunkCoord, mRootSeg, segmentsToDraw, mSegmentationID );

	delete this;
}
