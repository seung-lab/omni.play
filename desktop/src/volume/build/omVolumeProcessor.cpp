#include "volume/build/omVolumeProcessor.h"
#include "threads/taskManager.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omAffinityChannel.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"

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
    OmSegChunk* chunk = vol_.GetChunk(coord_);

    const bool isMIPzero = (0 == coord_.mipLevel());

    chunk->SegData()->ProcessChunk(isMIPzero, segments_);

    const auto segIDs = vol_.UniqueValuesDS().RereadChunk(coord_, 1);

    if (isMIPzero) {
      // vol_.updateMinMax(chunk->GetMinValue(),
      //                    chunk->GetMaxValue());
    }
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
