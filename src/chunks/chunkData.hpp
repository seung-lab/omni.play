#pragma once

#include "common/common.h"
#include "volume/volumeTypes.h"
#include "chunks/chunkDataInterface.hpp"
#include "chunks/chunkDataImpl.hpp"

namespace om {
namespace chunks {

class dataFactory {
public:
    template <typename VOL>
    static dataInterface* Produce(VOL* vol, const om::chunkCoord& coord)
    {
        switch(vol->getVolDataType().index()){
        case OmVolDataType::INT8:
            return new dataImpl<int8_t>(vol, coord);
        case OmVolDataType::UINT8:
            return new dataImpl<uint8_t>(vol, coord);
        case OmVolDataType::INT32:
            return new dataImpl<int32_t>(vol, coord);
        case OmVolDataType::UINT32:
            return new dataImpl<uint32_t>(vol, coord);
        case OmVolDataType::FLOAT:
            return new dataImpl<float>(vol, coord);
        case OmVolDataType::UNKNOWN:
        default:
            throw common::ioException("data type not handled");
        };
    }
};

} // namespace chunk
} // namespace om
