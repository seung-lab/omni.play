#pragma once
#include "precomp.h"

#include "view3d/mesh/drawer/struct.hpp"
#include "omMeshSegmentListTask.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

class OmSegment;
class OmSegments;
class OmSegmentation;

namespace om {
namespace v3d {
class MeshSegmentList;

class MeshSegmentListTask : public zi::runnable {
 private:
  MeshSegmentList& rootSegLists_;
  // om::chunk::CachedUniqueValuesDataSource& chunkUniqueValues_;
  OmChunkUniqueValuesManager& chunkUniqueValues_;
  OmSegments& segments_;

  const om::coords::Chunk coord_;
  OmSegment& rootSeg_;

  const om::v3d::key key_;

 public:
  MeshSegmentListTask(MeshSegmentList& rootSegLists,
                      // om::chunk::CachedUniqueValuesDataSource& cuvds,
                      OmChunkUniqueValuesManager& cuvds, OmSegments& segments,
                      om::coords::Chunk coord, OmSegment& rootSeg,
                      const om::v3d::key& key)
      : rootSegLists_(rootSegLists),
        chunkUniqueValues_(cuvds),
        segments_(segments),
        coord_(coord),
        rootSeg_(rootSeg),
        key_(key) {}

  void run();

 private:
  Vector3f determineColor(OmSegment*);
  bool useParentColor(OmSegment*);
};
}
}  // namespace
