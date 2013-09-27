#pragma once

#include "chunk/cachedDataSource.hpp"

namespace om {
namespace chunk {

template <typename T>
class VoxelGetter : public datalayer::IDataSource<coords::Global, T> {
 public:
  VoxelGetter(ChunkDS& chunkDS, const coords::VolumeSystem& coordSystem)
      : chunkDS_(chunkDS), coordSystem_(coordSystem) {}

  std::shared_ptr<T> Get(const coords::Global& key, bool async = false) {
    auto chunkCoord = key.ToChunk(coordSystem_, 0);
    auto chunk = chunkDS_.Get(chunkCoord);
    if (!chunk) {
      return std::shared_ptr<T>();
    }
    auto dataCoord = key.ToData(coordSystem_, 0);
    return std::make_shared<T>(boost::apply_visitor(
        chunk_extractor(dataCoord.ToChunkOffset()), *chunk));
  }

  T GetValue(const coords::Global& key) {
    auto chunkCoord = key.ToChunk(coordSystem_, 0);
    auto chunk = chunkDS_.Get(chunkCoord);
    auto dataCoord = key.ToData(coordSystem_, 0);
    return boost::apply_visitor(chunk_extractor(dataCoord.ToChunkOffset()),
                                *chunk);
  }

  bool Put(const coords::Global& key, std::shared_ptr<T> value,
           bool asnyc = false) {
    auto chunkCoord = key.ToChunk(coordSystem_, 0);
    auto chunk = chunkDS_.Get(chunkCoord);
    if (!chunk) {
      return false;
    }
    auto dataCoord = key.ToData(coordSystem_, 0);
    auto c = boost::get<Chunk<T>>(chunk.get());
    if (!c) {
      throw ArgException("Trying to write the wrong type of data to a volume.");
    }
    (*c)[dataCoord.ToChunkOffset()] = *value;
    chunkDS_.Put(chunkCoord, chunk);
    return true;
  }

  bool is_cache() const { return false; }
  bool is_persisted() const { return chunkDS_.is_persisted(); }

 private:
  struct chunk_extractor : public boost::static_visitor<T> {
    chunk_extractor(size_t offset) : offset(offset) {}

    template <typename TChunk>
    T operator()(const Chunk<TChunk>& chunk) const {
      throw ArgException("Trying to get the wrong type of data from a volume.");
    }

    T operator()(const Chunk<T>& chunk) const { return chunk[offset]; }

    size_t offset;
  };

  ChunkDS& chunkDS_;
  const coords::VolumeSystem& coordSystem_;
};
}
}  // namespace om::chunk::
