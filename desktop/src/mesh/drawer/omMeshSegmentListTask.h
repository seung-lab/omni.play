#pragma once
#include "precomp.h"
#include "coordinates/chunk.h"

class OmSegment;
class OmMeshSegmentList;
class OmSegmentation;

class OmMeshSegmentListTask : public zi::runnable {
 private:
  om::coords::Chunk chunkCoord_;
  OmSegment* const mRootSeg;
  OmMeshSegmentList* const rootSegLists_;
  OmSegmentation* const segmentation_;

 public:
  OmMeshSegmentListTask(om::coords::Chunk chunk, OmSegment* rootSeg,
                        OmMeshSegmentList* rootSegLists,
                        OmSegmentation* segmentation)
      : chunkCoord_(chunk),
        mRootSeg(rootSeg),
        rootSegLists_(rootSegLists),
        segmentation_(segmentation) {}

  void run();
};
