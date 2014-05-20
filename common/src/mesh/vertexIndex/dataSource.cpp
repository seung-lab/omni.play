#include "precomp.h"
#include "datalayer/filePtrCache.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include "mesh/vertexIndex/chunkTypes.hpp"
#include "mesh/vertexIndex/chunkWriter.hpp"
#include "mesh/dataSources.hpp"
#include "datalayer/vector.hpp"
#include "utility/malloc.hpp"
#include "datalayer/paths.hpp"

extern template class std::vector<uint32_t>;

namespace om {
namespace mesh {

class VertexIndexDataSourceImpl {
 public:
  inline datalayer::MemMappedFile<DataEntry> getAllocFile(
      const coords::Chunk& coord) const {
    return allocationTables_.Get(allocPath(coord));
  }

  DataEntry* getDataEntry(const datalayer::MemMappedFile<DataEntry>& table,
                          const coords::Mesh& coord) const {
    if (!table.IsMapped()) {
      log_errors << "Table is not mapped: " << table.GetBaseFileName();
      return nullptr;
    }

    DataEntry target = MakeEmptyEntry(coord.segID());
    DataEntry* entry =
        std::lower_bound(table.begin(), table.end(), target, compareBySegID);
    if (entry == table.end() || entry->segID != coord.segID()) {
      log_errors << "DataEntry not found in table.";
      return nullptr;
    }

    if (!entry || !entry->wasMeshed || !entry->hasMeshData) {
      log_errors << "DataEntry not found.";
      return nullptr;
    }

    return entry;
  }

  std::shared_ptr<VertexIndexMesh> getMesh(const coords::Mesh& coord) const {
    auto ret = std::make_shared<VertexIndexMesh>(coord);

    datalayer::MemMappedFile<DataEntry> table =
        getAllocFile(coord.mipChunkCoord());
    DataEntry* entry = getDataEntry(table, coord);

    if (!entry || !entry->wasMeshed) {
      return ret;
    }

    if (!entry->hasMeshData) {
      return ret;
    } else {
      ret->HasData(true);
    }

    auto dataFilePath = dataPath(coord.mipChunkCoord()).string();
    datalayer::MemMappedFile<char> dataFile = dataFiles_.Get(dataFilePath);

    ret->Data().SetVertexIndex(read<uint32_t>(entry->vertexIndex, dataFile),
                               entry->vertexIndex.count,
                               entry->vertexIndex.totalBytes);

    ret->Data().SetStripData(read<uint32_t>(entry->stripData, dataFile),
                             entry->stripData.count,
                             entry->stripData.totalBytes);

    ret->Data().SetVertexData(read<float>(entry->vertexData, dataFile),
                              entry->vertexData.count,
                              entry->vertexData.totalBytes);

    return ret;
  }

  bool putMesh(const coords::Mesh& coord,
               std::shared_ptr<VertexIndexMesh> mesh) {
    datalayer::MemMappedFile<DataEntry> table =
        getAllocFile(coord.mipChunkCoord());

    DataEntry* entry = getDataEntry(table, coord);

    if (!entry) {
      if (!file::exists(allocPath(coord.mipChunkCoord()))) {
        setupTableFile(coord.mipChunkCoord());
      } else {
        appendDataEntry(coord);
      }

      entry = getDataEntry(table, coord);
    }

    if (!entry) {
      throw IoException("Unable to create DataEntry");
    }

    entry->wasMeshed = true;
    entry->hasMeshData = mesh->HasData();

    auto dataFilePath = dataPath(coord.mipChunkCoord()).string();

    dataFiles_.Invalidate(dataFilePath);

    if (writers_.count(dataFilePath) == 0) {
      writers_[dataFilePath].reset(new ChunkWriter());
      writers_[dataFilePath]->Open(dataFilePath);
    }

    writers_[dataFilePath]->Append(entry->vertexIndex,
                                   mesh->Data().VertexIndex(),
                                   mesh->Data().VertexIndexNumBytes());
    writers_[dataFilePath]->Append(entry->stripData, mesh->Data().StripData(),
                                   mesh->Data().StripDataNumBytes());
    writers_[dataFilePath]->Append(entry->vertexData, mesh->Data().VertexData(),
                                   mesh->Data().VertexDataNumBytes());

    throw om::NotImplementedException("fixme--no return bool");
  }

  template <typename T>
  std::shared_ptr<T> read(const FilePart& part,
                          const datalayer::MemMappedFile<char>& rawData) const {
    const int64_t numBytes = part.totalBytes;

    assert(numBytes);

    auto ret = mem::Malloc<T>::NumBytes(numBytes, mem::ZeroFill::DONT);

    char* dataCharPtr = reinterpret_cast<char*>(ret.get());
    char* data = rawData.GetPtrWithOffset(part.offsetIntoFile);

    try {
      std::copy(data, &data[numBytes], dataCharPtr);
    }
    catch (std::exception e) {
      log_errors << "could not read mesh data: " << e.what();
      return std::shared_ptr<T>();
    }

    return ret;
  }

  static bool compareBySegID(const DataEntry& a, const DataEntry& b) {
    return a.segID < b.segID;
  }

  std::string relativePath(const coords::Chunk& key) const {
    return str(boost::format("%1%/%2%/%3%/%4%/") % key.mipLevel() % key.x %
               key.y % key.z);
  }

  std::string allocPath(const coords::Chunk& key) const {
    return (volumeRoot_ / file::Paths::Seg::MeshAllocTableRel(key)).string();
  }

  file::path dataPath(const coords::Chunk& key) const {
    return (volumeRoot_ / file::Paths::Seg::MeshDataRel(key)).string();
  }

  void appendDataEntry(const coords::Mesh& coord) {
    const std::string path = allocPath(coord.mipChunkCoord());
    allocationTables_.Invalidate(path);
    size_t newSize = file::numBytes(path) / sizeof(DataEntry) + 1;
    file::resizeFileNumElements<DataEntry>(path, newSize);

    datalayer::MemMappedFile<DataEntry> table = allocationTables_.Get(path);

    if (!table.IsMapped()) {
      throw IoException("Failed to remap file " + path);
    }

    DataEntry& newEntry = table.GetPtr()[newSize - 1];
    newEntry.segID = coord.segID();

    sortTable(table.GetPtr(), table.Length());
  }

  void setupTableFile(const coords::Chunk& coord) {
    const std::string path = allocPath(coord);
    const std::shared_ptr<chunk::UniqueValues> segIDs =
        uniqueValuesSource_->Get(coord);

    if (!segIDs || !segIDs.get()->Values.size()) {
      log_errors << "No unique values in " << coord;
      return;
    }

    allocationTables_.Invalidate(path);
    file::resizeFileNumElements<DataEntry>(path, segIDs->Values.size());

    datalayer::MemMappedFile<DataEntry> table = allocationTables_.Get(path);

    if (!table.IsMapped()) {
      throw IoException("Unable to create Allocation table: " + path);
    }

    resetTable(segIDs->Values, table.GetPtr());
    sortTable(table.GetPtr(), table.Length());
  }

  struct ResetEntry {
    DataEntry operator()(const om::common::SegID segID) const {
      return MakeEmptyEntry(segID);
    }
  };

  static void resetTable(const std::vector<uint32_t>& segIDs,
                         DataEntry* table) {
    assert(table);

    zi::transform(segIDs.begin(), segIDs.end(), table, ResetEntry());
  }

  static void sortTable(DataEntry* table, size_t numEntries) {
    assert(table);

    zi::sort(table, table + numEntries, compareBySegID);
  }

  VertexIndexDataSourceImpl(file::path volumeRoot,
                            chunk::UniqueValuesDS* uniqueValuesSource)
      : uniqueValuesSource_(uniqueValuesSource), volumeRoot_(volumeRoot) {}

  mutable datalayer::FilePtrCache<DataEntry> allocationTables_;
  mutable datalayer::FilePtrCache<char> dataFiles_;
  std::unordered_map<std::string, std::shared_ptr<ChunkWriter>> writers_;
  chunk::UniqueValuesDS* uniqueValuesSource_;
  file::path volumeRoot_;
};

VertexIndexDataSource::VertexIndexDataSource(
    file::path volumeRoot, chunk::UniqueValuesDS* uniqueValuesSource)
    : impl_(new VertexIndexDataSourceImpl(volumeRoot, uniqueValuesSource)) {}

VertexIndexDataSource::~VertexIndexDataSource() {}

std::shared_ptr<VertexIndexMesh> VertexIndexDataSource::Get(
    const coords::Mesh& coord, bool async) const {
  try {
    return impl_->getMesh(coord);
  }
  catch (...) {
    return std::shared_ptr<VertexIndexMesh>();
  }
}

bool VertexIndexDataSource::Put(const coords::Mesh& coord,
                                std::shared_ptr<VertexIndexMesh> mesh,
                                bool async) {
  try {
    return impl_->putMesh(coord, mesh);
  }
  catch (...) {
    return false;
  }
}
}
}  // namespace om::mesh::
