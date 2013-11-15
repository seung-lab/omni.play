#pragma once

#include "chunk/cachedDataSource.hpp"
#include "boost/multi_array.hpp"
#include <mutex>
#include <deque>

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
    if (!chunk) {
      return 0;
    }
    auto dataCoord = key.ToData(coordSystem_, 0);
    return boost::apply_visitor(chunk_extractor(dataCoord.ToChunkOffset()),
                                *chunk);
  }

  T GetValue(const coords::Data& dataCoord) {
    auto chunkCoord = dataCoord.ToChunk();
    auto chunk = chunkDS_.Get(chunkCoord);
    if (!chunk) {
      return 0;
    }
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

template <typename T>
class Voxels {
  typedef boost::multi_array<std::shared_ptr<ChunkVar>, 4> cache_t;
  typedef boost::multi_array<Chunk<T>*, 4> chunks_t;

 public:
  Voxels(ChunkDS& chunkDS, const coords::VolumeSystem& coordSystem,
         size_t cacheSize = 100)
      : chunkDS_(chunkDS),
        coordSystem_(coordSystem),
        cacheSize_(cacheSize),
        cache_(boost::extents[coordSystem_.RootMipLevel()]
                             [coordSystem_.ChunkDimensions().x]
                             [coordSystem_.ChunkDimensions().y]
                             [coordSystem_.ChunkDimensions().z]),
        chunks_(boost::extents[coordSystem_.RootMipLevel()]
                              [coordSystem_.ChunkDimensions().x]
                              [coordSystem_.ChunkDimensions().y]
                              [coordSystem_.ChunkDimensions().z]) {}

  T GetValue(const coords::Global& key) {
    auto chunkCoord = key.ToChunk(coordSystem_, 0);
    auto c = chunk(chunkCoord);
    if (!c) {
      return 0;
    }
    auto dataCoord = key.ToData(coordSystem_, 0);
    return (*c)[dataCoord.ToChunkOffset()];
  }

  T GetValue(const coords::Data& key) {
    auto chunkCoord = key.ToChunk();
    auto c = chunk(chunkCoord);
    if (!c) {
      return 0;
    }
    return (*c)[key.ToChunkOffset()];
  }

 private:
  Chunk<T>* chunk(const coords::Chunk& cc) {
    auto& chunkref = index(chunks_, cc);
    if (chunkref) {
      return chunkref;
    }

    {
      std::lock_guard<std::mutex> g(mutex_);
      auto& cacheref = index(cache_, cc);
      cacheref = chunkDS_.Get(cc);
      if (!cacheref) {
        return nullptr;
      }

      chunkref = boost::get<Chunk<T>>(cacheref.get());
      cacheQueue_.push_back(cc);
      if (cacheQueue_.size() > cacheSize_) {
        auto toPop = cacheQueue_.front();
        index(cache_, toPop) = std::shared_ptr<ChunkVar>();
        index(chunks_, toPop) = nullptr;
        cacheQueue_.pop_front();
      }
    }
    return chunkref;
  }

  template <typename TVal>
  TVal& index(boost::multi_array<TVal, 4>& coll, const coords::Chunk& cc) {
    return coll[cc.mipLevel()][cc.x][cc.y][cc.z];
  }

  ChunkDS& chunkDS_;
  const coords::VolumeSystem& coordSystem_;
  const size_t cacheSize_;
  cache_t cache_;
  chunks_t chunks_;
  std::deque<coords::Chunk> cacheQueue_;
  std::mutex mutex_;
};
}
}  // namespace om::chunk::
