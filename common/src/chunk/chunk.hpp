#pragma once

#include <boost/variant.hpp>

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "utility/malloc.hpp"

namespace om {
namespace chunk {

template <typename T> class Chunk {
 public:
  Chunk(coords::Chunk coord, coords::VolumeSystem vol)
      : coord_(coord),
        vol_(vol),
        dims_(vol.ChunkDimensions()),
        data_(mem::Malloc<T>::NumElements(dims_.x * dims_.y * dims_.z,
                                          mem::ZeroFill::DONT)) {}

  Chunk(coords::Chunk coord, coords::VolumeSystem vol, std::shared_ptr<T> data)
      : coord_(coord), vol_(vol), dims_(vol.ChunkDimensions()), data_(data) {}

  const T& operator[](uint i) const {
    assert(i < dims_.x * dims_.y * dims_.z);
    return data_.get()[i];
  }

  T& operator[](uint i) {
    assert(i < dims_.x * dims_.y * dims_.z);
    return data_.get()[i];
  }

  std::shared_ptr<T> data() const { return data_; }

  inline size_t length() const { return dims_.x * dims_.y * dims_.z; }

  inline size_t size() const { return length() * sizeof(T); }

 private:
  PROP_CONST_REF(coords::Chunk, coord);
  PROP_CONST_REF(coords::VolumeSystem, vol);
  PROP_CONST_REF(Vector3i, dims);

 private:  // PROP_CONST_REF appears to use protected...
  std::shared_ptr<T> data_;
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
