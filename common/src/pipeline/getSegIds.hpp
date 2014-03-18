#pragma once

#include "precomp.h"
#include "common/common.h"
#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class getSegId : public boost::static_visitor<uint32_t> {
 private:
  coords::Data coord_;

 public:
  getSegId(coords::Data coord) : coord_(coord) {}

  template <typename T>
  uint32_t operator()(const datalayer::MemMappedFile<T>& in) const {
    uint64_t offset = coord_.ToChunk().PtrOffset(coord_.volume(), sizeof(T));
    T* chunkPtr = in.GetPtrWithOffset(offset);
    return chunkPtr[coord_.ToChunkOffset()];
  }
};

uint32_t operator>>(const dataSrcs& d, const getSegId& v) {
  return boost::apply_visitor(v, d);
}
}
}
