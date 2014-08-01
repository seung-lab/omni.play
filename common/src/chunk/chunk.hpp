#pragma once

#include "precomp.h"

#include "common/common.h"
#include "common/indexedIterator.hpp"
#include "coordinates/coordinates.h"
#include "utility/malloc.hpp"
#include <mutex>

namespace om {
namespace chunk {

template <typename T>
class Chunk {
 public:
  Chunk(coords::Chunk coord, coords::VolumeSystem vol)
      : data_(mem::Malloc<T>::NumElements(dims_.x * dims_.y * dims_.z,
                                          mem::ZeroFill::DONT)),
        coord_(coord),
        vol_(vol),
        dims_(vol.ChunkDimensions()) {}

  Chunk(coords::Chunk coord, coords::VolumeSystem vol, std::shared_ptr<T> data)
      : data_(data), coord_(coord), vol_(vol), dims_(vol.ChunkDimensions()) {}

  const T& operator[](uint i) const {
    assert(i < dims_.x * dims_.y * dims_.z);
    return data_.get()[i];
  }

  T& operator[](uint i) {
    assert(i < dims_.x * dims_.y * dims_.z);
    return data_.get()[i];
  }

  std::shared_ptr<T> data() const { return data_; }

  size_t length() const { return dims_.x * dims_.y * dims_.z; }
  size_t size() const { return length() * sizeof(T); }

  typedef indexed_iterator<Chunk<T>, T> iterator;
  typedef indexed_iterator<const Chunk<T>, const T> const_iterator;
  iterator begin() { return iterator(*this, 0); }
  iterator end() { return iterator(*this, length()); }
  const_iterator cbegin() const { return const_iterator(*this, 0); }
  const_iterator cend() const { return const_iterator(*this, length()); }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

 private:
  std::shared_ptr<T> data_;

  PROP_CONST_REF(coords::Chunk, coord);
  PROP_CONST_REF(coords::VolumeSystem, vol);
  PROP_CONST_REF(Vector3i, dims);
};

extern template class Chunk<int8_t>;
extern template class Chunk<uint8_t>;
extern template class Chunk<int32_t>;
extern template class Chunk<uint32_t>;
extern template class Chunk<float>;

typedef boost::variant<Chunk<int8_t>, Chunk<uint8_t>, Chunk<int32_t>,
                       Chunk<uint32_t>, Chunk<float>> ChunkVar;
}
}  // namespace om::chunk::

extern template class indexed_iterator<om::chunk::Chunk<int8_t>, int8_t>;
extern template class indexed_iterator<om::chunk::Chunk<uint8_t>, uint8_t>;
extern template class indexed_iterator<om::chunk::Chunk<int32_t>, int32_t>;
extern template class indexed_iterator<om::chunk::Chunk<uint32_t>, uint32_t>;
extern template class indexed_iterator<om::chunk::Chunk<float>, float>;
extern template class indexed_iterator<om::chunk::Chunk<const int8_t>,
                                       const int8_t>;
extern template class indexed_iterator<om::chunk::Chunk<const uint8_t>,
                                       const uint8_t>;
extern template class indexed_iterator<om::chunk::Chunk<const int32_t>,
                                       const int32_t>;
extern template class indexed_iterator<om::chunk::Chunk<const uint32_t>,
                                       const uint32_t>;
extern template class indexed_iterator<om::chunk::Chunk<const float>,
                                       const float>;