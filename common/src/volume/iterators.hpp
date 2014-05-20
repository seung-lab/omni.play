#pragma once

#include "chunk/iterators.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace volume {
template <typename T, typename ChunkIterator>
class dataval_iterator;

template <typename T>
class CoordValue {
 public:
  boost::optional<CoordValue<T>> neighbor(Vector3i offset) const {
    // TODO: Speed up!
    if (!chunk_) {
      return boost::optional<CoordValue<T>>();
    }

    auto ret = *this;
    ret.coord_ += offset;

    auto* chunk = chunk_;
    if (ret.coord_.ToChunk() != coord_.ToChunk()) {
      auto chunkPtr = chunkDs_.get().Get(ret.coord_.ToChunk());
      chunk = boost::get<chunk::Chunk<T>*>(chunkPtr.get());
      if (!chunk) {
        return boost::optional<CoordValue<T>>();
      }
    }
    ret.Value = (*chunk)[ret.coord_.ToChunkOffset()];

    return ret;
  }

 private:
  CoordValue(chunk::ChunkDS& ds, coords::Data coord)
      : coord_(coord), chunkDs_(std::ref(ds)) {
    updateChunk();
    updateValue(coord_.ToChunkOffset());
  }

  PROP_REF(coords::Data, coord);
  PROP_REF(T, value);

  void updateChunk(const coords::Chunk& cc) {
    chunkSharedPtr_ = chunkDs_.get().Get(cc);
    auto typedChunk = boost::get<chunk::Chunk<T>>(chunkSharedPtr_.get());
    if (typedChunk) {
      chunk_ = typedChunk->data().get();
    } else {
      chunk_ = nullptr;
    }
    if (!chunk_) {
      log_errors << "Unable to get chunk " << cc;
    }
  }

  void updateValue(size_t idx) { value_ = chunk_[idx]; }

  T* chunk_;
  std::shared_ptr<chunk::ChunkVar> chunkSharedPtr_;
  std::reference_wrapper<chunk::ChunkDS> chunkDs_;

  template <typename, typename>
  friend class dataval_iterator;
};

template <typename T, typename ChunkIterator>
class dataval_iterator
    : public boost::iterator_adaptor<
          dataval_iterator<T, ChunkIterator>, ChunkIterator, CoordValue<T>,
          std::forward_iterator_tag, const CoordValue<T>&> {
  typedef typename dataval_iterator<T, ChunkIterator>::iterator_adaptor_ base_t;

 public:
  dataval_iterator() : base_t() {}
  dataval_iterator(ChunkIterator chunkIter, const coords::DataBbox& bounds,
                   chunk::ChunkDS& ds, const coords::Data& curr)
      : base_t(chunkIter), iterBounds_(new coords::DataBbox(bounds)) {
    // TODO: What if the chunkIter has bad bounds?  Better way to initialize
    // without knowing the type?

    if (!bounds.contains(curr)) {
      throw ArgException("Invalid curr or bounds");
    }
    val_.reset(new CoordValue<T>(ds, curr));
    idx_ = val_->coord_.ToChunkOffset();

    updateChunkBounds();
  }

  dataval_iterator(const dataval_iterator& other)
      : base_t(other),
        idx_(other.idx_),
        chunkFrom_(other.chunkFrom_),
        chunkTo_(other.chunkTo_) {
    if ((bool)other.iterBounds_) {
      iterBounds_.reset(new coords::DataBbox(*other.iterBounds_));
    }
    if ((bool)other.val_) {
      val_.reset(new CoordValue<T>(*other.val_));
    }
  }

  dataval_iterator(dataval_iterator&& other)
      : base_t(other),
        idx_(other.idx_),
        iterBounds_(std::move(other.iterBounds_)),
        chunkFrom_(other.chunkFrom_),
        chunkTo_(other.chunkTo_),
        val_(std::move(other.val_)) {}

 private:
  friend class boost::iterator_core_access;

  const CoordValue<T>& dereference() const { return *val_; }

  bool equal(const dataval_iterator& y) const { return *val_ == *y.val_; }

  void increment() {
    if (++val_->coord_.x > chunkTo_.x) {
      val_->coord_.x = chunkFrom_.x;
      if (++val_->coord_.y > chunkTo_.y) {
        val_->coord_.y = chunkFrom_.y;
        if (++val_->coord_.z > chunkTo_.z) {
          base_t::base_reference()++;
          if (base_t::base() == ChunkIterator()) {
            val_->chunk_ = nullptr;
            return;
          } else {
            updateChunkBounds();
            val_->coord_ = coords::Data(chunkFrom_, val_->coord_.volume(),
                                        val_->coord_.mipLevel());
            val_->updateChunk();
          }
        }
      }
      idx_ = val_->coord_.ToChunkOffset();
    } else {
      idx_++;
    }

    val_->updateValue(idx_);
  }
  void decrement() {
    if (--val_->coord_.x < chunkFrom_.x) {
      val_->coord_.x = chunkTo_.x;
      if (--val_->coord_.y < chunkFrom_.y) {
        val_->coord_.y = chunkTo_.y;
        if (--val_->coord_.z < chunkFrom_.z) {
          base_t::base_reference()--;
          if (base_t::base() == ChunkIterator()) {
            val_->chunk_ = nullptr;
            return;
          } else {
            updateChunkBounds();
            val_->coord_ = coords::Data(chunkTo_, val_->coord_.volume(),
                                        val_->coord_.mipLevel());
            val_->updateChunk();
          }
        }
      }
      idx_ = val_->coord_.ToChunkOffset();
    } else {
      idx_--;
    }
    val_->updateValue(idx_);
  }

  void updateChunkBounds() {
    auto cb = base_t::base()->BoundingBox(val_->coord_.volume());
    cb.intersect(*iterBounds_);
    chunkFrom_ = cb.getMin();
    chunkTo_ = cb.getMax();
  }

  int idx_;
  std::unique_ptr<coords::DataBbox> iterBounds_;
  Vector3i chunkFrom_, chunkTo_;

  std::unique_ptr<CoordValue<T>> val_;
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
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, common::SegID id) {
  return segment_filtered_dataval_iterator<T>(
      [id](const std::pair<coords::Data, T>& p) { return p.second == id; },
      make_chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, id));
}

template <typename T>
segment_filtered_dataval_iterator<T> make_segment_filtered_dataval_iterator(
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, common::SegIDSet set) {
  return segment_filtered_dataval_iterator<T>(
      [set](const std::pair<coords::Data, T>& p) {
        return set.count(p.second);
      },
      make_chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, set));
}
}
}  // namespace om::volume::
