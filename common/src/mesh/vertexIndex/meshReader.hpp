#pragma once

#include "common/common.h"
#include "mesh/vertexIndex/data.hpp"
#include "mesh/vertexIndex/meshChunkDataReader.hpp"
#include "mesh/vertexIndex/memMappedAllocFile.hpp"
#include "datalayer/paths.hpp"

namespace om {
namespace mesh {

class reader {
 private:
  std::unique_ptr<memMappedAllocFile> chunkTable_;
  std::unique_ptr<chunkDataReader> chunkData_;

 public:
  reader(file::path volPath, const coords::Chunk& coord) {
    file::path allocTablePath =
        volPath / file::Paths::Seg::MeshAllocTable(coord);
    if (!file::exists(allocTablePath)) {
      throw ArgException("Alloc table file not found.");
    }

    chunkTable_.reset(new memMappedAllocFile(allocTablePath));

    file::path dataPath = volPath / file::Paths::Seg::MeshData(coord);
    if (!file::exists(dataPath)) {
      throw ArgException("Mesh data file not found.");
    }

    chunkData_.reset(new chunkDataReader(dataPath));
  }

  ~reader() {}

  const DataEntry* GetDataEntry(const common::SegID segId) {
    return chunkTable_->Find(MakeEmptyEntry(segId));
  }

  std::shared_ptr<VertexIndexData> Read(const common::SegID segId) {
    auto ret = std::make_shared<VertexIndexData>();

    const DataEntry* entry = GetDataEntry(segId);

    if (!entry || !entry->wasMeshed) {
      return ret;
    }

    if (!entry->hasMeshData) {
      return ret;
    } else {
      ret->HasData(true);
    }

    ret->SetVertexIndex(chunkData_->Read<uint32_t>(entry->vertexIndex),
                        entry->vertexIndex.count,
                        entry->vertexIndex.totalBytes);

    ret->SetStripData(chunkData_->Read<uint32_t>(entry->stripData),
                      entry->stripData.count, entry->stripData.totalBytes);

    ret->SetVertexData(chunkData_->Read<float>(entry->vertexData),
                       entry->vertexData.count, entry->vertexData.totalBytes);

    if (entry->trianData.totalBytes) {
      ret->SetTrianData(chunkData_->Read<uint32_t>(entry->trianData),
                        entry->trianData.count, entry->trianData.totalBytes);
    }

    return ret;
  }
};

}  // namespace mesh
}  // namespace om
