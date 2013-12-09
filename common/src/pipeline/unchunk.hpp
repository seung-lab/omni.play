#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class unchunk  //: public stage
    {
 private:
  const coords::VolumeSystem& vs_;
  const Vector3i dims_;

 public:
  unchunk(const coords::VolumeSystem& vs)
      : vs_(vs), dims_(vs_.DataDimensions()) {}

  inline size_t target_offset(coords::Data d) const {
    return d.x + ((size_t) d.y) * dims_.x + ((size_t) d.z) * dims_.x * dims_.y;
  }

  template <typename T>
  Data<T> operator()(const datalayer::MemMappedFile<T>& in) const {
    Data<T> out(((size_t) dims_.x) * dims_.y * dims_.z);
    T* outPtr = out.data.get();

    const Vector3i dims = vs_.ChunkDimensions();

    // iterate over all chunks in order
    // TODO: boost multi_array
    auto chunks = vs_.MipChunkCoords(0);
    for (auto& coord : *chunks) {
      coords::Data base = coord.ToData(vs_);
      uint64_t offset = coord.PtrOffset(vs_, sizeof(T));
      T* chunkPtr = in.GetPtrWithOffset(offset);

      for (auto z = 0; z < dims.z; ++z) {
        for (auto y = 0; y < dims.y; ++y) {
          coords::Data d(base.x, base.y + y, base.z + z, vs_, 0);
          if (!d.IsInVolume()) {
            break;
          }
          size_t off = d.ToChunkOffset();
          size_t end = off + std::min(dims.x, dims_.x - base.x);
          std::copy(&chunkPtr[off], &chunkPtr[end], &outPtr[target_offset(d)]);
        }
      }
    }
    return out;
  }
};

// data_var operator>>(const dataSrcs& d, const unchunk& v) {
// boost::apply_visitor(v, d);
// }
}
}
