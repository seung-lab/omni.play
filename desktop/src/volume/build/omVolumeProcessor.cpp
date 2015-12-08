#include "volume/build/omVolumeProcessor.h"
#include "threads/taskManager.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omAffinityChannel.h"
#include "segment/omSegments.h"
#include "volume/build/omProcessSegmentationChunk.hpp"
#include "chunk/chunkUtils.hpp"

class OmSegmentationChunkBuildTask : public zi::runnable {
 private:
  const om::coords::Chunk coord_;
  OmSegmentation& vol_;
  OmSegments& segments_;

 public:
  OmSegmentationChunkBuildTask(const om::coords::Chunk& coord,
                               OmSegments& segments, OmSegmentation& vol)
      : coord_(coord), vol_(vol), segments_(segments) {}

  void run() {
    auto chunk = vol_.GetChunk(coord_);

    // We need to compute the sizes and bounding box for only MIP level 0
    bool computeSizeAndBounds = coord_.mipLevel() == 0;
    OmProcessSegmentationChunk p(computeSizeAndBounds, computeSizeAndBounds, segments_);
    for (auto& dv : vol_.Iterate<uint32_t>(coord_.BoundingBox(vol_.Coords()))) {
      if (dv.value()) {
        p.processVoxel(dv.value(), dv.coord());
      }
    }

    auto uv = om::chunk::utils::MakeUniqueValues(vol_.ChunkDS(), coord_,
                                                 vol_.Coords());
    vol_.UniqueValuesDS().Put(coord_, uv);
  }
};

void OmVolumeProcessor::doBuildThreadedVolume(OmSegmentation& vol) {
  om::thread::ThreadPool threadPool;
  threadPool.start();

  std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
      vol.GetMipChunkCoords();

  for(auto coord: *coordsPtr) {

    auto task = std::make_shared<OmSegmentationChunkBuildTask>(coord, vol.Segments(), vol);
    threadPool.push_back(task);
  }

  threadPool.join();
}

void OmVolumeProcessor::doBuildThreadedVolume(OmChannel&) {}

void OmVolumeProcessor::doBuildThreadedVolume(OmAffinityChannel&) {}
