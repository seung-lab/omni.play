#include "common/omCommon.h"
#include "mesh/drawer/omMeshSegmentList.hpp"
#include "mesh/drawer/omMeshSegmentListTask.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "events/omEvents.h"
#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

void OmMeshSegmentListTask::run()
{
    const ChunkUniqueValues chunkSegIDs =
        segmentation_->ChunkUniqueValues()->Values(mChunk->GetCoordinate(), 1);

    OmSegmentIterator segIter(mRootSeg->Segments());
    segIter.iterOverSegmentID(mRootSeg->value());
    OmSegment* seg = segIter.getNextSegment();

    OmSegPtrList segmentsToDraw;

    while( NULL != seg ){
        if(chunkSegIDs.contains(seg->value())){
            segmentsToDraw.push_back(seg);
        }

        seg = segIter.getNextSegment();
    }

    rootSegLists_->AddToCache(mChunk, mRootSeg, segmentsToDraw);
    OmEvents::Redraw3d();
    //	printf("done..(%u)\n", mRootSeg->value);
}
