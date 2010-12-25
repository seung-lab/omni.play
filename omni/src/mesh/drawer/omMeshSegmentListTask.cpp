#include "common/omCommon.h"
#include "mesh/drawer/omMeshSegmentList.hpp"
#include "mesh/drawer/omMeshSegmentListTask.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "system/omEvents.h"
#include "volume/omMipChunk.h"

void OmMeshSegmentListTask::run()
{
	const OmSegIDsSet& chunkValues = mChunk->GetUniqueSegIDs();
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

	rootSegLists_->AddToCache(mChunk, mRootSeg, segmentsToDraw);
	OmEvents::Redraw3d();
	//	printf("done..(%u)\n", mRootSeg->value);
}
