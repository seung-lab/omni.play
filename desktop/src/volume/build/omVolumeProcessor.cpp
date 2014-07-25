#include "volume/build/omVolumeProcessor.h"
#include "threads/taskManager.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omAffinityChannel.h"
#include "segment/omSegments.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "volume/build/omProcessSegmentationChunk.hpp"

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

    OmProcessSegmentationChunk p(coord_.mipLevel() == 0, segments_);

    for (auto& dv : vol_.Iterate<uint32_t>(coord_.BoundingBox(vol_.Coords()))) {
      if (dv.value()) {
        p.processVoxel(dv.value(), dv.coord());
      }
    }

    const auto segIDs = vol_.UniqueValuesDS().RereadChunk(coord_, 1);
  }
};

void OmVolumeProcessor::doBuildThreadedVolume(OmSegmentation& vol) {
  om::thread::ThreadPool threadPool;
  threadPool.start();

  std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
      vol.GetMipChunkCoords();

  FOR_EACH(iter, *coordsPtr) {
    const om::coords::Chunk& coord = *iter;

    std::shared_ptr<OmSegmentationChunkBuildTask> task =
        std::make_shared<OmSegmentationChunkBuildTask>(coord, vol.Segments(),
                                                       vol);
    threadPool.push_back(task);
  }

  threadPool.join();
}

void OmVolumeProcessor::doBuildThreadedVolume(OmChannel&) {}

void OmVolumeProcessor::doBuildThreadedVolume(OmAffinityChannel&) {}
