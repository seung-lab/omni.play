#pragma once

#include "chunk/cachedDataSource.hpp"
#include "boost/multi_array.hpp"
#include <mutex>
#include <deque>

namespace om {
namespace chunk {

template <typename T>
class Voxels {
  typedef boost::multi_array<std::shared_ptr<ChunkVar>, 4> cache_t;
  typedef boost::multi_array<Chunk<T>*, 4> chunks_t;

 public:
  Voxels(ChunkDS& chunkDS, const coords::VolumeSystem& coordSystem,
         size_t cacheSize = 25)
      : chunkDS_(chunkDS),
        coordSystem_(coordSystem),
        cacheSize_(cacheSize),
        cache_(boost::extents[coordSystem_.RootMipLevel() + 1]
                             [coordSystem_.ChunkDimensions().x]
                             [coordSystem_.ChunkDimensions().y]
                             [coordSystem_.ChunkDimensions().z]),
        chunks_(boost::extents[coordSystem_.RootMipLevel() + 1]
                              [coordSystem_.ChunkDimensions().x]
                              [coordSystem_.ChunkDimensions().y]
                              [coordSystem_.ChunkDimensions().z]) {}

  T GetValue(const coords::Global& key) {
    return GetValue(key.ToData(coordSystem_, 0));
  }

  T GetValue(const coords::Data& key) {
    assert(&key.volume() == &coordSystem_);
    if (!coordSystem_.Contains(key)) {
      return 0;
    }
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
