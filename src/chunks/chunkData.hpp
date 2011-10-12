#pragma once

#include "common/common.h"
#include "datalayer/dataWrapper.h"
#include "volume/volumeTypes.h"
#include "chunks/chunkDataInterface.hpp"
#include "chunks/chunkDataImpl.hpp"

namespace om {
namespace chunk {

class dataFactory {
public:
    template <typename VOL>
    static dataInterface* Produce(VOL* vol, const coords::chunkCoord& coord)
    {
        switch(vol->getVolDataType().index()){
        case volume::dataType::INT8:
            return new dataImpl<int8_t>(vol, coord);
        case volume::dataType::UINT8:
            return new dataImpl<uint8_t>(vol, coord);
        case volume::dataType::INT32:
            return new dataImpl<int32_t>(vol, coord);
        case volume::dataType::UINT32:
            return new dataImpl<uint32_t>(vol, coord);
        case volume::dataType::FLOAT:
            return new dataImpl<float>(vol, coord);
        case volume::dataType::UNKNOWN:
        default:
            throw common::ioException("data type not handled");
        };
    }
};

} // namespace chunk
} // namespace om
