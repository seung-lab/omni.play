#pragma once

#include "common/common.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"
#include "chunks/omChunkDataInterface.hpp"
#include "chunks/omChunkDataImpl.hpp"

namespace om {
namespace chunk {

class dataFactory {
public:
    template <typename VOL>
    static dataInterface* Produce(VOL* vol, const om::coords::Chunk& coord)
    {
        switch(vol->getVolDataType().index()){
        case om::common::DataType::INT8:
            return new dataImpl<int8_t>(vol, coord);
        case om::common::DataType::UINT8:
            return new dataImpl<uint8_t>(vol, coord);
        case om::common::DataType::INT32:
            return new dataImpl<int32_t>(vol, coord);
        case om::common::DataType::UINT32:
            return new dataImpl<uint32_t>(vol, coord);
        case om::common::DataType::FLOAT:
            return new dataImpl<float>(vol, coord);
        case om::common::DataType::UNKNOWN:
        default:
            throw IoException("data type not handled");
        };
    }
};

} // namespace chunk
} // namespace om
