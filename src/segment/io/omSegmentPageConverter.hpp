#pragma once

#include "utility/omSmartPtr.hpp"
#include "segment/segmentTypes.h"

class segmentPageConverter {
public:
    om::shared_ptr<segmentDataV4>
    static ConvertPageV3toV4(om::shared_ptr<segmentDataV3> dataV3,
                             const uint32_t pageSize)
    {
        segmentDataV3* rawV3 = dataV3.get();

        om::shared_ptr<segmentDataV4> ret =
            OmSmartPtr<segmentDataV4>::MallocNumElements(pageSize, om::ZERO_FILL);
        segmentDataV4* rawV4 = ret.get();

        for(uint32_t i = 0; i < pageSize; ++i)
        {
            rawV4[i].value  = rawV3[i].value;
            rawV4[i].color  = rawV3[i].color;
            rawV4[i].size   = rawV3[i].size;
            rawV4[i].bounds = rawV3[i].bounds;
        }

        return ret;
    }

    om::shared_ptr<uint8_t>
    static ConvertPageV3toV4ListType(om::shared_ptr<segmentDataV3> dataV3,
                                     const uint32_t pageSize)
    {
        segmentDataV3* rawV3 = dataV3.get();

        om::shared_ptr<uint8_t> ret =
            OmSmartPtr<uint8_t>::MallocNumElements(pageSize, om::ZERO_FILL);
        uint8_t* raw = ret.get();

        for(uint32_t i = 0; i < pageSize; ++i)
        {
            raw[i] = rawV3[i].listType;
        }

        return ret;
    }
};

