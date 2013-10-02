#pragma once

#include "coordinates/chunk.h"
#include "mesh/drawer/struct.hpp"
#include "chunk/dataSources.hpp"

class OmSegment;
class OmSegments;
class OmMeshSegmentList;
class OmSegmentation;

class OmMeshSegmentListTask : public zi::runnable {
 private:
  OmMeshSegmentList& rootSegLists_;
  om::chunk::UniqueValuesDS& chunkUniqueValues_;
  OmSegments& segments_;

  const om::coords::Chunk coord_;
  OmSegment& rootSeg_;

  const om::v3d::key key_;

 public:
  OmMeshSegmentListTask(OmMeshSegmentList& rootSegLists,
                        om::chunk::UniqueValuesDS& chunkUniqueValues,
                        OmSegments& segments, om::coords::Chunk coord,
                        OmSegment& rootSeg, const om::v3d::key& key)
      : rootSegLists_(rootSegLists),
        chunkUniqueValues_(chunkUniqueValues),
        segments_(segments),
        coord_(coord),
        rootSeg_(rootSeg),
        key_(key) {}

  void run();

 private:
  Vector3f determineColor(OmSegment*);
  bool useParentColor(OmSegment*);
};
