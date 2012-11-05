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
                                  const om::coords::Chunk& coord)
    {
        switch(vol->getVolDataType().index()){
        case om::common::DataType::INT8:
            return new dataImpl<int8_t>(vol, chunk, coord);

        case om::common::DataType::UINT8:
            return new dataImpl<uint8_t>(vol, chunk, coord);

        case om::common::DataType::INT32:
            return new dataImpl<int32_t>(vol, chunk, coord);

        case om::common::DataType::UINT32:
            return new dataImpl<uint32_t>(vol, chunk, coord);

        case om::common::DataType::FLOAT:
            return new dataImpl<float>(vol, chunk, coord);

        case om::common::DataType::UNKNOWN:
        default:
            throw IoException("data type not handled");
        };
    }
};

} // namespace segchunk
} // namespace om


