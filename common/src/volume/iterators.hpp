#pragma once

#include "chunk/iterators.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace volume {
template <typename T, typename ChunkIterator>
class dataval_iterator
    : public boost::iterator_adaptor<dataval_iterator<T, ChunkIterator>,
                                     ChunkIterator, std::pair<coords::Data, T>,
                                     std::forward_iterator_tag,
                                     const std::pair<coords::Data, T>&> {
  typedef typename dataval_iterator<T, ChunkIterator>::iterator_adaptor_ base_t;

 public:
  dataval_iterator() : base_t(), chunk_(nullptr) {}
  dataval_iterator(ChunkIterator chunkIter, const coords::DataBbox& bounds,
                   chunk::ChunkDS& ds, const coords::Data& curr)
      : base_t(chunkIter),
        iterBounds_(new coords::DataBbox(bounds)),
        chunkDs_(&ds),
        val_(curr, 0) {
    // TODO: What if the chunkIter has bad bounds?  Better way to initialize
    // without knowing the type?

    updateChunkBounds();
    updateChunk();
    val_.first =
        coords::Data(chunkFrom_, val_.first.volume(), val_.first.mipLevel());

    idx_ = val_.first.ToChunkOffset();
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
        val_(other.val_) {}

  boost::optional<std::pair<coords::Data, T>> neighbor(Vector3i offset) const {
    // TODO: Speed up!
    if (!chunkDs_) {
      return false;
    }

    std::pair<coords::Data, T> ret = val_;
    ret.first += offset;

    if (ret.ToChunk() == val_.ToChunk()) {
      if (!chunk_) {
        return false;
      }

      ret.second = chunk_[ret.first.ToChunkOffset()];
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
    return val_;
  }

  bool equal(const dataval_iterator& y) const {
    if (!y.chunk_) {
      return !chunk_;
    } else {
      return chunk_ && val_ == y.val_;
    }
  }

  void increment() {
    if (++val_.first.x > chunkTo_.x) {
      val_.first.x = chunkFrom_.x;
      if (++val_.first.y > chunkTo_.y) {
        val_.first.y = chunkFrom_.y;
        if (++val_.first.z > chunkTo_.z) {
          base_t::base_reference()++;
          if (base_t::base() == ChunkIterator()) {
            chunk_ = nullptr;
            return;
          } else {
            updateChunkBounds();
            val_.first = coords::Data(chunkFrom_, val_.first.volume(),
                                      val_.first.mipLevel());
            updateChunk();
          }
        }
      }
      idx_ = val_.first.ToChunkOffset();
    } else {
      idx_++;
    }

    setDataVal();
  }
  void decrement() {
    if (--val_.first.x < chunkFrom_.x) {
      val_.first.x = chunkTo_.x;
      if (--val_.first.y < chunkFrom_.y) {
        val_.first.y = chunkTo_.y;
        if (--val_.first.z < chunkFrom_.z) {
          base_t::base_reference()--;
          if (base_t::base() == ChunkIterator()) {
            chunk_ = nullptr;
            return;
          } else {
            updateChunkBounds();
            val_.first = coords::Data(chunkTo_, val_.first.volume(),
                                      val_.first.mipLevel());
            updateChunk();
          }
        }
      }
      idx_ = val_.first.ToChunkOffset();
    } else {
      idx_--;
    }
    setDataVal();
  }

  void updateChunk() {
    chunkSharedPtr_ = chunkDs_->Get(*base_t::base());
    auto typedChunk = boost::get<chunk::Chunk<T>>(chunkSharedPtr_.get());
    if (typedChunk) {
      chunk_ = typedChunk->data().get();
    } else {
      chunk_ = nullptr;
    }
    if (!chunk_) {
      log_errors << "Unable to get chunk " << *base_t::base();
    }
  }

  void updateChunkBounds() {
    auto cb = base_t::base()->BoundingBox(val_.first.volume());
    cb.intersect(*iterBounds_);
    chunkFrom_ = cb.getMin();
    chunkTo_ = cb.getMax();
  }

  void setDataVal() { val_.second = chunk_[idx_]; }

  int idx_;
  std::unique_ptr<coords::DataBbox> iterBounds_;
  Vector3i chunkFrom_, chunkTo_;
  T* chunk_;
  std::shared_ptr<chunk::ChunkVar> chunkSharedPtr_;
  chunk::ChunkDS* chunkDs_;
  mutable std::pair<coords::Data, T> val_;
};

template <typename T>
using all_dataval_iterator = dataval_iterator<T, chunk::iterator>;

template <typename T>
all_dataval_iterator<T> make_all_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds) {
  return all_dataval_iterator<T>(
      chunk::make_iterator(bounds.getMin().ToChunk(),
                           bounds.getMax().ToChunk()),
      bounds, ds, bounds.getMin());
}

template <typename T>
using chunk_filtered_dataval_iterator =
    dataval_iterator<T, chunk::filtered_iterator>;

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegID id) {
  return chunk_filtered_dataval_iterator<T>(
      chunk::make_segment_iterator(bounds.getMin().ToChunk(),
                                   bounds.getMax().ToChunk(), uvm, id),
      bounds, ds, bounds.getMin());
}

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegIDSet set) {
  return chunk_filtered_dataval_iterator<T>(
      chunk::make_segset_iterator(bounds.getMin().ToChunk(),
                                  bounds.getMax().ToChunk(), uvm, set),
      bounds, ds, bounds.getMin());
}

template <typename T>
using segment_filtered_dataval_iterator = boost::filter_iterator<
    std::function<bool(const std::pair<coords::Data, T>&)>,
    chunk_filtered_dataval_iterator<T>>;

template <typename T>
segment_filtered_dataval_iterator<T> make_segment_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegID id) {
  return segment_filtered_dataval_iterator<T>(
      [id](const std::pair<coords::Data, T>& p) { return p.second == id; },
      chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, id));
}

template <typename T>
segment_filtered_dataval_iterator<T> make_segment_filtered_dataval_iterator(
    const coords::DataBbox& bounds, chunk::ChunkDS& ds,
    chunk::UniqueValuesDS& uvm, common::SegIDSet set) {
  return segment_filtered_dataval_iterator<T>(
      [set](const std::pair<coords::Data, T>& p) {
        return set.count(p.second);
      },
      chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, set));
}
}
}  // namespace om::volume::
