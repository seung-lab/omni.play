#pragma once

#include "utility/malloc.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageConverter {
public:
    std::shared_ptr<OmSegmentDataV4>
    static ConvertPageV3toV4(std::shared_ptr<OmSegmentDataV3> dataV3,
                             const uint32_t pageSize)
    {
        OmSegmentDataV3* rawV3 = dataV3.get();

        std::shared_ptr<OmSegmentDataV4> ret =
            om::mem::Malloc<OmSegmentDataV4>::NumElements(pageSize, om::mem::ZeroFill::ZERO);
        OmSegmentDataV4* rawV4 = ret.get();

        for(uint32_t i = 0; i < pageSize; ++i)
        {
            rawV4[i].value  = rawV3[i].value;
            rawV4[i].color  = rawV3[i].color;
            rawV4[i].size   = rawV3[i].size;
            rawV4[i].bounds = rawV3[i].bounds;
        }

        return ret;
    }

    std::shared_ptr<uint8_t>
    static ConvertPageV3toV4ListType(std::shared_ptr<OmSegmentDataV3> dataV3,
                                     const uint32_t pageSize)
    {
        OmSegmentDataV3* rawV3 = dataV3.get();

        std::shared_ptr<uint8_t> ret =
            om::mem::Malloc<uint8_t>::NumElements(pageSize, om::mem::ZeroFill::ZERO);
        uint8_t* raw = ret.get();

        for(uint32_t i = 0; i < pageSize; ++i)
        {
            raw[i] = static_cast<int>(rawV3[i].listType);
        }

        return ret;
    }
};

