#pragma once

#include "zi/omThreads.h"

class OmSegment;
class OmMeshSegmentList;
class OmSegmentation;

class OmMeshSegmentListTask : public zi::runnable {
 private:
  OmSegChunk* mChunk;
  OmSegment* const mRootSeg;
  OmMeshSegmentList* const rootSegLists_;
  OmSegmentation* const segmentation_;

 public:
  OmMeshSegmentListTask(OmSegChunk* p_chunk, OmSegment* rootSeg,
                        OmMeshSegmentList* rootSegLists,
                        OmSegmentation* segmentation)
      : mChunk(p_chunk),
        mRootSeg(rootSeg),
        rootSegLists_(rootSegLists),
        segmentation_(segmentation) {}

  void run();
};
