#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "datalayer/memMappedFile.hpp"
#include "coordinates/coordinates.h"
#include "chunk/chunk.hpp"

namespace om {
namespace pipeline {

class getChunk : public stage {
 public:
  template <typename T> data_var operator()(const chunk::Chunk<T>& in) const {
    return Data<T>(in.data(), in.length());
  }
};

data_var operator>>(const chunk::ChunkVar& d, const getChunk& v);

class writeChunk : boost::static_visitor<> {
 private:
  dataSrcs file_;
  coords::Data dc_;

 public:
  writeChunk(dataSrcs file, coords::Data dc) : file_(file), dc_(dc) {}

  template <typename T> void operator()(const Data<T>& toWrite) {
    auto* file = boost::get<datalayer::MemMappedFile<T>>(file_);
    if (!file) {
      throw ArgException("Trying to write the wrong type of data.");
    }

    if (toWrite.size != 128 * 128 * 128) {
      throw ArgException("Chunk Data is the wrong size.");
    }

    coords::Chunk cc = dc_.ToChunk();
    uint64_t offset = cc.PtrOffset(dc_.volume(), sizeof(T));
    T* chunkPtr = file->GetPtrWithOffset(offset);

    std::copy(toWrite->data.get(), &toWrite->data.get()[toWrite->size],
              chunkPtr);
  }
};
}
}
