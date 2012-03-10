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
    static dataInterface* Produce(volume::segmentation* vol, chunks::segChunk* chunk,
                                  const coords::chunkCoord& coord)
    {
        switch(vol->getVolDataType().index()){
        case volume::dataType::INT8:
            return new dataImpl<int8_t>(vol, chunk, coord);

        case volume::dataType::UINT8:
            return new dataImpl<uint8_t>(vol, chunk, coord);

        case volume::dataType::INT32:
            return new dataImpl<int32_t>(vol, chunk, coord);

        case volume::dataType::UINT32:
            return new dataImpl<uint32_t>(vol, chunk, coord);

        case volume::dataType::FLOAT:
            return new dataImpl<float>(vol, chunk, coord);

        case volume::dataType::UNKNOWN:
        default:
            throw common::ioException("data type not handled");
        };
    }
};

} // namespace segchunk
} // namespace om


