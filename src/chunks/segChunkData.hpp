#pragma once

#include "common/common.h"
#include "chunks/segChunkDataInterface.hpp"
#include "chunks/segChunkDataImpl.hpp"

class segChunk;
class segmentation;

namespace om {
namespace segchunk {

class dataFactory {
public:
    static dataInterface* Produce(segmentation* vol, segChunk* chunk,
                                  const om::chunkCoord& coord)
    {
        switch(vol->getVolDataType().index()){
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
            throw OmIoException("data type not handled");
        };
    }
};

} // namespace segchunk
} // namespace om


