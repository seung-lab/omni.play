#include "chunk/dataSource.h"
#include "chunk/chunk.hpp"
#include "utility/malloc.hpp"
#include "datalayer/paths.hpp"

namespace om {
namespace chunk {

using namespace pipeline;

FileDataSource::FileDataSource(file::Paths::Vol volPaths,
                               const common::DataType& dataType,
                               const coords::VolumeSystem& coordSystem)
    : volPaths_(volPaths), dataType_(dataType), coordSystem_(coordSystem) {}

class ChunkSlicer
    : public boost::static_visitor<std::shared_ptr<chunk::ChunkVar>> {
 public:
  ChunkSlicer(coords::Chunk coord, const coords::VolumeSystem& system)
      : coord_(coord), system_(system) {}

  template <typename T>
  std::shared_ptr<chunk::ChunkVar> operator()(
      const datalayer::MemMappedFile<T>& in) const {
    if (!in.IsMapped()) {
      throw ArgException("Invalid volume: " + in.GetBaseFileName().string());
    }
    int size = system_.ChunkDimensions().x * system_.ChunkDimensions().y *
               system_.ChunkDimensions().z;
    uint64_t offset = coord_.PtrOffset(system_, sizeof(T));
    T* chunkPtr = in.GetPtrWithOffset(offset);

    auto data = mem::Malloc<T>::NumElements(size, mem::ZeroFill::DONT);
    std::copy(chunkPtr, &chunkPtr[size], data.get());

    return std::make_shared<chunk::ChunkVar>(
        chunk::Chunk<T>(coord_, system_, data));
  }

 private:
  coords::Chunk coord_;
  const coords::VolumeSystem& system_;
};

std::string FileDataSource::fileName(int mipLevel) const {
  return volPaths_.Data(mipLevel, dataType_).string();
}

std::shared_ptr<chunk::ChunkVar> FileDataSource::Get(const coords::Chunk& coord,
                                                     bool async) const {
  try {
    auto file = files_.Get(fileName(coord.mipLevel()), dataType_);
    return boost::apply_visitor(ChunkSlicer(coord, coordSystem_), file);
  }
  catch (Exception e) {

    log_errors << "Unable to read from ChunkDS: " << e.what();
    return std::shared_ptr<chunk::ChunkVar>();
  }
}

class ChunkWriter : public boost::static_visitor<void> {
 public:
  ChunkWriter(coords::Chunk coord, const coords::VolumeSystem& system)
      : coord_(coord), system_(system) {}

  template <typename T>
  void operator()(datalayer::MemMappedFile<T>& in,
                  chunk::Chunk<T>& chunk) const {
    if (!in.IsMapped()) {
      throw InvalidOperationException("Invalid volume: " +
                                      in.GetBaseFileName().string());
    }
    uint64_t offset = coord_.PtrOffset(system_, sizeof(T));
    T* chunkPtr = in.GetPtrWithOffset(offset);

    std::copy(chunk.begin(), chunk.end(), chunkPtr);
  }

  template <typename T, typename U>
  void operator()(datalayer::MemMappedFile<T>&, chunk::Chunk<U>&) const {
    throw InvalidOperationException("Writing incorrect type of data to volume");
  }

 private:
  coords::Chunk coord_;
  const coords::VolumeSystem& system_;
};

bool FileDataSource::Put(const coords::Chunk& coord,
                         std::shared_ptr<chunk::ChunkVar> chunk, bool async) {
  try {
    auto file = files_.Get(fileName(coord.mipLevel()), dataType_);
    boost::apply_visitor(ChunkWriter(coord, coordSystem_), file, *chunk);
    return true;
  }
  catch (Exception e) {
    log_errors << "Unable to write to ChunkDS: " << e.what();
    return false;
  }
}
}
}  // namespace om::chunk::
