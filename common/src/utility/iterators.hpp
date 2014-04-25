#pragma once

#include "precomp.h"
#include "common/common.h"
#include "coordinates/chunk.h"
#include "chunk/voxelGetter.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace utility {

// Iterates through a 3d range of vector values [from, to] inclusive.
template <typename T>
class vector3_iterator
    : public boost::iterator_facade<vector3_iterator<T>, const Vector3<T>,
                                    std::bidirectional_iterator_tag> {
 public:
  vector3_iterator()
      : from_(Vector3<T>::ZERO),
        to_(Vector3<T>::ZERO),
        val_(Vector3<T>::ZERO),
        valid_(false) {}
  vector3_iterator(Vector3<T> from, Vector3<T> to, Vector3<T> curr)
      : from_(from), to_(to), val_(curr), valid_(from <= curr && curr <= to) {
    if (!valid_) {
      log_errors << "Invalid iterator: " << from << "<=" << curr << "<=" << to;
    }
  }
  vector3_iterator(Vector3<T> from, Vector3<T> to)
      : vector3_iterator(from, to, from) {}
  vector3_iterator(const vector3_iterator& other)
      : from_(other.from_),
        to_(other.to_),
        val_(other.val_),
        valid_(other.valid_) {}

 private:
  friend class boost::iterator_core_access;

  const Vector3<T>& dereference() const { return val_; }

  bool equal(const vector3_iterator& y) const {
    return (!y.valid_ && !valid_) || (y.valid_ && val_ == y.val_);
  }

  void increment() {
    if (++val_.z > to_.z) {
      val_.z = from_.z;
      if (++val_.y > to_.y) {
        val_.y = from_.y;
        if (++val_.x > to_.x) {
          val_.x = from_.x;
          valid_ = false;
        }
      }
    }
  }

  void decrement() {
    if (--val_.z < from_.z) {
      val_.z = to_.z;
      if (--val_.y < from_.y) {
        val_.y = to_.y;
        if (--val_.x < from_.x) {
          val_.x = to_.x;
          valid_ = false;
        }
      }
    }
  }

  Vector3<T> from_, to_;
  Vector3<T> val_;
  bool valid_;
};

///  CHUNK ITERATORS //////////////////////////

// Iterates over chunks in a given range at a given mip level.  Built using the
// vector_iterator.
typedef boost::transform_iterator<std::function<coords::Chunk(const Vector3i&)>,
                                  vector3_iterator<int>> chunk_iterator;

inline chunk_iterator make_chunk_iterator(int mipLevel,
                                          vector3_iterator<int> iter) {
  return chunk_iterator(iter, [mipLevel](const Vector3i& vec) {
    return coords::Chunk(mipLevel, vec);
  });
}

inline chunk_iterator make_chunk_iterator(const coords::Chunk& from,
                                          const coords::Chunk& to) {
  return make_chunk_iterator(from.mipLevel(), vector3_iterator<int>(from, to));
}

// Iterates over specific chunks in a given range at a given mip level. Takes a
// filter function to decide what chunks to use. Built using the chunk_iterator.
typedef boost::filter_iterator<std::function<bool(const coords::Chunk&)>,
                               chunk_iterator> filtered_chunk_iterator;

inline filtered_chunk_iterator make_filtered_chunk_iterator(
    std::function<bool(const coords::Chunk&)> filter, chunk_iterator iter) {
  return filtered_chunk_iterator(filter, iter);
}

inline filtered_chunk_iterator make_filtered_chunk_iterator(
    std::function<bool(const coords::Chunk&)> filter, const coords::Chunk& from,
    const coords::Chunk& to) {
  return make_filtered_chunk_iterator(filter, make_chunk_iterator(from, to));
}

// Iterates over specific chunks in a given range at a given mip level.  This
// only yeilds chunks which contain segments in the given set.  Built using the
// filtered_chunk_iterator.
inline filtered_chunk_iterator make_segment_chunk_iterator(
    chunk::UniqueValuesDS& uvm, common::SegID segment, chunk_iterator iter) {
  return make_filtered_chunk_iterator([&uvm, segment](const coords::Chunk& c) {
                                        auto uv = uvm.Get(c);
                                        return !uv || uv->contains(segment);
                                      },
                                      iter);
}

inline filtered_chunk_iterator make_segment_chunk_iterator(
    const coords::Chunk& from, const coords::Chunk& to,
    chunk::UniqueValuesDS& uvm, common::SegID segment) {
  return make_segment_chunk_iterator(uvm, segment,
                                     make_chunk_iterator(from, to));
}

// Iterates over specific chunks in a given range at a given mip level.  This
// only yeilds chunks which contain segments in the given set.  Built using the
// filtered_chunk_iterator.
inline filtered_chunk_iterator make_segset_chunk_iterator(
    chunk::UniqueValuesDS& uvm, common::SegIDSet segs, chunk_iterator iter) {
  return make_filtered_chunk_iterator([&uvm, segs](const coords::Chunk& c) {
                                        auto uv = uvm.Get(c);
                                        return !uv ||
                                               uv->contains_any(segs.begin(),
                                                                segs.end());
                                      },
                                      iter);
}

inline filtered_chunk_iterator make_segset_chunk_iterator(
    const coords::Chunk& from, const coords::Chunk& to,
    chunk::UniqueValuesDS& uvm, common::SegIDSet segs) {
  return make_segset_chunk_iterator(uvm, segs, make_chunk_iterator(from, to));
}

/// DATAVAL ITERATORS /////////////////////////

template <typename T, typename ChunkIterator>
class dataval_iterator
    : public boost::iterator_adaptor<dataval_iterator<T, ChunkIterator>,
                                     ChunkIterator, std::pair<coords::Data, T>,
                                     std::forward_iterator_tag,
                                     const std::pair<coords::Data, T>&> {
  typedef typename dataval_iterator<T, ChunkIterator>::iterator_adaptor_ base_t;

 public:
  dataval_iterator() : base_t() {}
  dataval_iterator(ChunkIterator chunkIter, const coords::DataBbox& bounds,
                   chunk::ChunkDS& ds, const coords::Data& curr)
      : base_t(chunkIter),
        iterBounds_(new coords::DataBbox(bounds)),
        chunkDs_(&ds),
        val_(new std::pair<coords::Data, T>(curr, 0)) {
    // TODO: What if the chunkIter has bad bounds?  Better way to initialize
    // without knowing the type?

    updateChunkBounds();
    updateChunk();

    idx_ = val_->first.ToChunkOffset();
    setDataVal();
  }

  dataval_iterator(const dataval_iterator& other)
      : base_t(other),
        idx_(other.idx_),
        iterBounds_(new coords::DataBbox(*other.iterBounds_)),
        chunkFrom_(other.chunkFrom_),
        chunkTo_(other.chunkTo_),
        chunk_(other.chunk_),
        chunkDs_(other.chunkDs_),
        val_(new std::pair<coords::Data, T>(*other.val_)) {}

  boost::optional<std::pair<coords::Data, T>> neighbor(Vector3i offset) const {
    // TODO: Speed up!
    if (!chunkDs_) {
      return false;
    }

    std::pair<coords::Data, T> ret = *val_;
    ret.first += offset;

    if (ret.ToChunk() == val_->ToChunk()) {
      if (!chunk_) {
        return false;
      }

      ret.second = (*chunk_)[ret.first.ToChunkOffset()];
    } else {
      auto chunkPtr = chunkDs_->Get(ret.ToChunk());
      auto chunk = boost::get<chunk::Chunk<T>*>(chunkPtr.get());
      if (!chunk) {
        return false;
      }

      ret.second = (*chunk)[ret.first.ToChunkOffset()];
    }

    return ret;
  }

 private:
  friend class boost::iterator_core_access;

  const std::pair<coords::Data, common::SegID>& dereference() const {
    return *val_;
  }

  bool equal(const dataval_iterator& y) const {
    if (!y.val_) {
      return !val_;
    } else {
      return val_ && *val_ == *y.val_;
    }
  }

  void increment() {
    auto& ref = *val_;
    if (++ref.first.x >= chunkTo_.x) {
      ref.first.x = chunkFrom_.x;
      if (++ref.first.y >= chunkTo_.y) {
        ref.first.y = chunkFrom_.y;
        if (++ref.first.z >= chunkTo_.z) {
          base_t::base_reference()++;
          if (base_t::base() == ChunkIterator()) {
            val_.reset();
          } else {
            updateChunkBounds();
            ref.first = coords::Data(chunkFrom_, ref.first.volume(),
                                     ref.first.mipLevel());
            updateChunk();
          }
        }
      }
      idx_ = ref.first.ToChunkOffset();
    } else {
      idx_++;
    }

    setDataVal();
  }
  void decrement() {
    auto& ref = *val_;
    if (--ref.first.x < chunkFrom_.x) {
      ref.first.x = chunkTo_.x;
      if (--ref.first.y < chunkFrom_.y) {
        ref.first.y = chunkTo_.y;
        if (--ref.first.z < chunkFrom_.z) {
          base_t::base_reference()--;
          if (base_t::base() == ChunkIterator()) {
            val_.reset();
          } else {
            updateChunkBounds();
            ref.first = coords::Data(chunkTo_, ref.first.volume(),
                                     ref.first.mipLevel());
            updateChunk();
          }
        }
      }
      idx_ = ref.first.ToChunkOffset();
    } else {
      idx_--;
    }
    setDataVal();
  }

  void updateChunk() {
    chunkSharedPtr_ = chunkDs_->Get(*base_t::base());
    chunk_ = boost::get<chunk::Chunk<T>>(chunkSharedPtr_.get());
    if (!chunk_) {
      log_errors << "Unable to get chunk " << *base_t::base();
      val_.reset();
    }
  }

  void updateChunkBounds() {
    auto cb = base_t::base()->BoundingBox(val_->first.volume());
    cb.intersect(*iterBounds_);
    chunkFrom_ = cb.getMin();
    chunkTo_ = cb.getMax();
  }

  void setDataVal() {
    if (val_) {
      val_->second = (*chunk_)[idx_];
    }
  }

  int idx_;
  std::unique_ptr<coords::DataBbox> iterBounds_;
  Vector3i chunkFrom_, chunkTo_;
  chunk::Chunk<T>* chunk_;
  std::shared_ptr<chunk::ChunkVar> chunkSharedPtr_;
  chunk::ChunkDS* chunkDs_;
  mutable std::unique_ptr<std::pair<coords::Data, T>> val_;
};

template <typename T>
using all_dataval_iterator = dataval_iterator<T, chunk_iterator>;

template <typename T>
all_dataval_iterator<T> make_all_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds) {
  return all_dataval_iterator<T>(
      make_chunk_iterator(bounds.getMin().ToChunk(), bounds.getMax().ToChunk()),
      bounds, ds, bounds.getMin());
}

template <typename T>
using chunk_filtered_dataval_iterator =
    dataval_iterator<T, filtered_chunk_iterator>;

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegID id) {
  return chunk_filtered_dataval_iterator<T>(
      make_segment_chunk_iterator(bounds.getMin().ToChunk(),
                                  bounds.getMax().ToChunk(), uvm, id),
      bounds, ds, bounds.getMin());
}

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegIDSet set) {
  return chunk_filtered_dataval_iterator<T>(
      make_segset_chunk_iterator(bounds.getMin().ToChunk(),
                                 bounds.getMax().ToChunk(), uvm, set),
      bounds, ds, bounds.getMin());
}
}
}  // namespace om::utility::