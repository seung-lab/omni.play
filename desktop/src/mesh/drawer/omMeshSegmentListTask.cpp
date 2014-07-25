#include "common/common.h"
#include "mesh/drawer/omMeshSegmentList.hpp"
#include "mesh/drawer/omMeshSegmentListTask.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "events/events.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

void OmMeshSegmentListTask::run() {
  const ChunkUniqueValues chunkSegIDs =
      segmentation_->UniqueValuesDS().Values(chunkCoord_, 1);

  OmSegmentIterator segIter(segmentation_->Segments());
  segIter.iterOverSegmentID(mRootSeg->value());
  OmSegment* seg = segIter.getNextSegment();

  OmSegPtrList segmentsToDraw;

  while (nullptr != seg) {
    if (chunkSegIDs.contains(seg->value())) {
      segmentsToDraw.push_back(seg);
    }

    seg = segIter.getNextSegment();
  }

  rootSegLists_->AddToCache(chunkCoord_, mRootSeg, segmentsToDraw);
  om::event::Redraw3d();
}
