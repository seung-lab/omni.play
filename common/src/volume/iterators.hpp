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

    if (ret.coord_.ToChunk() != coord_.ToChunk()) {
      ret.updateChunk();
    }
    ret.updateValue();
    return ret;
  }

  friend bool operator==(const CoordValue& a, const CoordValue& b) {
    return std::tie(a.coord_, a.value_) == std::tie(b.coord_, b.value_);
  }

 private:
  CoordValue(const chunk::ChunkDS& ds, coords::Data coord)
      : coord_(coord), chunkDs_(std::ref(ds)) {
    updateChunk(coord.ToChunk());
    if (chunk_) {
      updateValue(coord_.ToChunkOffset());
    }
  }

  PROP_REF(coords::Data, coord);
  PROP_REF(T, value);

  void updateChunk(const coords::Chunk& cc) {
    chunkSharedPtr_ = chunkDs_.get().Get(cc);
    auto typedChunk = boost::get<chunk::Chunk<T>>(chunkSharedPtr_.get());
    chunk_ = typedChunk ? typedChunk->data().get() : nullptr;
    if (!chunk_) {
      log_errors << "Unable to get chunk " << cc;
    }
  }
  void updateChunk() { updateChunk(coord_.ToChunk()); }

  void updateValue(size_t idx) { value_ = chunk_[idx]; }
  void updateValue() { updateValue(coord_.ToChunkOffset()); }

  T* chunk_;
  std::shared_ptr<chunk::ChunkVar> chunkSharedPtr_;
  std::reference_wrapper<const chunk::ChunkDS> chunkDs_;

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
                   const chunk::ChunkDS& ds)
      : base_t(chunkIter), iterBounds_(new coords::DataBbox(bounds)) {
    // TODO: What if the chunkIter has bad bounds?  Better way to initialize
    // without knowing the type?

    auto chunkBounds = base_t::base()->BoundingBox(bounds.volume());

    val_.reset(new CoordValue<T>(ds, chunkBounds.getMin() < bounds.getMin()
                                         ? bounds.getMin()
                                         : chunkBounds.getMin()));
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

  bool equal(const dataval_iterator& y) const {
    if (!val_ || !y.val_) {
      return val_.get() == y.val_.get();
    } else {
      return *val_ == *y.val_;
    }
  }

  void increment() {
    if (++val_->coord_.x > chunkTo_.x) {
      val_->coord_.x = chunkFrom_.x;
      if (++val_->coord_.y > chunkTo_.y) {
        val_->coord_.y = chunkFrom_.y;
        if (++val_->coord_.z > chunkTo_.z) {
          base_t::base_reference()++;
          if (base_t::base() == ChunkIterator()) {
            val_.reset();
            return;
          } else {
            updateChunkBounds();
            val_->coord_ = coords::Data(chunkFrom_, val_->coord_.volume(),
                                        val_->coord_.mipLevel());
            val_->updateChunk(*base_t::base());
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
            val_.reset();
            return;
          } else {
            updateChunkBounds();
            val_->coord_ = coords::Data(chunkTo_, val_->coord_.volume(),
                                        val_->coord_.mipLevel());
            val_->updateChunk(*base_t::base());
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
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds) {
  return all_dataval_iterator<T>(
      chunk::make_iterator(bounds.getMin().ToChunk(),
                           bounds.getMax().ToChunk()),
      bounds, ds);
}

template <typename T>
using chunk_filtered_dataval_iterator =
    dataval_iterator<T, chunk::filtered_iterator>;

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, common::SegID id) {
  return chunk_filtered_dataval_iterator<T>(
      chunk::make_segment_iterator(bounds.getMin().ToChunk(),
                                   bounds.getMax().ToChunk(), uvm, id),
      bounds, ds);
}

template <typename T>
chunk_filtered_dataval_iterator<T> make_chunk_filtered_dataval_iterator(
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, common::SegIDSet set) {
  return chunk_filtered_dataval_iterator<T>(
      chunk::make_segset_iterator(bounds.getMin().ToChunk(),
                                  bounds.getMax().ToChunk(), uvm, set),
      bounds, ds);
}

template <typename T>
using segment_filtered_dataval_iterator =
    boost::filter_iterator<std::function<bool(const CoordValue<T>& p)>,
                           chunk_filtered_dataval_iterator<T>>;

template <typename T>
segment_filtered_dataval_iterator<T> make_segment_filtered_dataval_iterator(
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, T id) {
  return segment_filtered_dataval_iterator<T>(
      [id](const CoordValue<T>& p) { return p.value() == id; },
      make_chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, id));
}

template <typename T>
segment_filtered_dataval_iterator<T> make_segment_filtered_dataval_iterator(
    const coords::DataBbox& bounds, const chunk::ChunkDS& ds,
    const chunk::UniqueValuesDS& uvm, std::set<T> set) {
  return segment_filtered_dataval_iterator<T>(
      [set](const CoordValue<T>& p) { return set.count(p.value()); },
      make_chunk_filtered_dataval_iterator<T>(bounds, ds, uvm, set));
}
}
}  // namespace om::volume::
