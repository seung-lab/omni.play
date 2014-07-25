#include "segment/omSegments.h"
#include "threads/taskManager.hpp"
#include "volume/omSegmentation.h"
#include "volume/omUpdateBoundingBoxes.h"
#include "volume/build/omProcessSegmentationChunk.hpp"

OmUpdateBoundingBoxes::OmUpdateBoundingBoxes(OmSegmentation* vol)
    : vol_(vol), segments_(vol->Segments()) {}

void OmUpdateBoundingBoxes::doUpdate(const om::coords::Chunk& coord) {
  auto chunk = vol_->GetChunk(coord);

  OmProcessSegmentationChunk p(false, vol_->Segments());

  for (auto& dv : vol_->Iterate<uint32_t>(coord.BoundingBox(vol_->Coords()))) {
    if (dv.value()) {
      p.processVoxel(dv.value(), dv.coord());
    }
  }
}

void OmUpdateBoundingBoxes::Update() {
  om::thread::ThreadPool pool;
  pool.start();

  std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
      vol_->GetMipChunkCoords(0);

  FOR_EACH(iter, *coordsPtr) {
    pool.push_back(
        zi::run_fn(zi::bind(&OmUpdateBoundingBoxes::doUpdate, this, *iter)));
  }

  pool.join();
}
