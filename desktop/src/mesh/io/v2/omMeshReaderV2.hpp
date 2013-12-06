#pragma once

#include "common/common.h"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataReaderV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/omMeshManager.h"

class OmMeshReaderV2 {
 private:
  OmSegmentation* const segmentation_;
  const double threshold_;
  OmMeshFilePtrCache* const filePtrCache_;

 public:
  OmMeshReaderV2(OmMeshManager* meshManager)
      : segmentation_(meshManager->GetSegmentation()),
        threshold_(meshManager->Threshold()),
        filePtrCache_(meshManager->FilePtrCache()) {}

  ~OmMeshReaderV2() {}

  inline std::shared_ptr<OmDataForMeshLoad> Read(const OmMeshCoord& meshCoord) {
    return Read(meshCoord.SegID(), meshCoord.Coord());
  }

  std::shared_ptr<OmDataForMeshLoad> Read(const om::common::SegID segID,
                                          const om::chunkCoord& coord) {
    OmMeshChunkAllocTableV2* chunk_table = filePtrCache_->GetAllocTable(coord);

    OmMeshChunkDataReaderV2 chunk_data(segmentation_, coord, threshold_);

    std::shared_ptr<OmDataForMeshLoad> ret =
        std::make_shared<OmDataForMeshLoad>();

    if (!chunk_table->Contains(segID)) {
      return ret;
    }

    const OmMeshDataEntry entry = chunk_table->Find(segID);

    if (!entry.wasMeshed) {
      log_infos << "Warning: did not yet mesh " << segID << " in coord "
                << coord;
      return ret;
    }

    ret->HasData(entry.hasMeshData);

    if (!entry.hasMeshData) {
      return ret;
    }

    ret->SetVertexIndex(chunk_data.Read<uint32_t>(entry.vertexIndex),
                        entry.vertexIndex.count, entry.vertexIndex.totalBytes);

    ret->SetStripData(chunk_data.Read<uint32_t>(entry.stripData),
                      entry.stripData.count, entry.stripData.totalBytes);

    ret->SetVertexData(chunk_data.Read<float>(entry.vertexData),
                       entry.vertexData.count, entry.vertexData.totalBytes);

    if (entry.trianData.totalBytes) {
      ret->SetTrianData(chunk_data.Read<uint32_t>(entry.trianData),
                        entry.trianData.count, entry.trianData.totalBytes);
    }

    return ret;
  }
};
