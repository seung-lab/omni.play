#pragma once

#include "mesh/io/v1/omMeshReaderV1.hpp"
#include "mesh/io/v2/omMeshWriterV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "system/cache/omCacheManager.h"
#include "zi/omThreads.h"

class OmMeshConvertV1toV2Task : public zi::runnable {
 private:
  OmMeshManager* const meshManager_;
  OmSegmentation* const segmentation_;
  const double threshold_;

  std::unique_ptr<OmMeshReaderV1> hdf5Reader_;
  std::unique_ptr<OmMeshWriterV2> meshWriter_;

 public:
  OmMeshConvertV1toV2Task(OmMeshManager* meshManager)
      : meshManager_(meshManager),
        segmentation_(meshManager->GetSegmentation()),
        threshold_(meshManager->Threshold()),
        hdf5Reader_(new OmMeshReaderV1(segmentation_)),
        meshWriter_(new OmMeshWriterV2(meshManager)) {}

  void run() {
    log_infos << "copying mesh data...";

    std::shared_ptr<std::deque<om::chunkCoord> > coordsPtr =
        segmentation_->GetMipChunkCoords();

    FOR_EACH(iter, *coordsPtr) {
      const om::chunkCoord& coord = *iter;

      if (!processChunk(coord)) {
        return;
      }
    }

    meshWriter_->Join();

    meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();

    log_infos << "mesh conversion done!";
  }

 private:
  bool processChunk(const om::chunkCoord& coord) {
    const ChunkUniqueValues segIDs =
        segmentation_->UniqueValuesDS().Values(coord, 1);

    FOR_EACH(segID, segIDs) {
      if (OmCacheManager::AmClosingDown()) {
        // TODO: note that mesh conversion was not done?
        return false;
      }

      if (meshWriter_->WasMeshed(*segID, coord)) {
        continue;
      }

      auto mesh = hdf5Reader_->Read(*segID, coord);

      meshWriter_->Save(*segID, coord, mesh,
                        om::common::ShouldBufferWrites::DONT_BUFFER_WRITES,
                        om::common::AllowOverwrite::WRITE_ONCE);
    }

    return true;
  }
};
