#pragma once

#include "common/exception.h"
#include "datalayer/file.h"

namespace om {
namespace handler {

inline void validateMetadata(const server::metadata& meta) {
  if (!file::exists(meta.uri)) {
    throw argException("Cannot find requested volu2me.");
  }
  /*
      int factor = math::pow2int(meta.mipLevel);
  
      std::size_t size = (meta.bounds.max.x - meta.bounds.min.x) *
          (meta.bounds.max.y - meta.bounds.min.y) *
          (meta.bounds.max.z - meta.bounds.min.z) /
          (factor * factor * factor);
  
      switch(meta.type)
      {
      case server::dataType::INT32:
      case server::dataType::UINT32:
      case server::dataType::FLOAT:
          size *= 4;
          break;
      }
  
      if(file::numBytes(meta.uri) != size) {
          throw argException("Incorrect file size given bounds, mip level and
      data type.");
      }
  */

  if (meta.resolution.x <= 0 || meta.resolution.y <= 0 ||
      meta.resolution.z <= 0) {
    throw argException("Resolution must be greater than 0.");
  }

  if (meta.chunkDims.x <= 0 || meta.chunkDims.y <= 0 || meta.chunkDims.z <= 0) {
    throw argException("Chunk Dims must be greater than 0.");
  }
}

}
}
