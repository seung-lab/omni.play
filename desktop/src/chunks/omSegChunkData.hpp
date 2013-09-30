#pragma once

#include "common/common.h"
#include "chunks/omSegChunkDataInterface.hpp"
#include "chunks/omSegChunkDataImpl.hpp"

class OmSegChunk;
class OmSegmentation;

namespace om {
namespace segchunk {

class dataFactory {
 public:
  static dataInterface* Produce(OmSegmentation* vol, OmSegChunk* chunk,
                                const om::chunkCoord& coord) {
    switch (vol->getVolDataType().index()) {
      case OmVolDataType::INT8:
        return new dataImpl<int8_t>(vol, chunk, coord);

      case OmVolDataType::UINT8:
        return new dataImpl<uint8_t>(vol, chunk, coord);

      case OmVolDataType::INT32:
        return new dataImpl<int32_t>(vol, chunk, coord);

      case OmVolDataType::UINT32:
        return new dataImpl<uint32_t>(vol, chunk, coord);

      case OmVolDataType::FLOAT:
        return new dataImpl<float>(vol, chunk, coord);

      case OmVolDataType::UNKNOWN:
      default:
        throw om::IoException("data type not handled");
    }
    ;
  }
};

}  // namespace segchunk
}  // namespace om
