#ifndef OM_SEGMENT_PAGE_CONVERTER_HPP
#define OM_SEGMENT_PAGE_CONVERTER_HPP

#include "utility/omSmartPtr.hpp"
#include "segment/omSegmentTypes.h"

class OmSegmentPageConverter {
public:
    boost::shared_ptr<OmSegmentDataV4>
    static ConvertPageV3toV4(boost::shared_ptr<OmSegmentDataV3> dataV3,
                             const uint32_t pageSize)
    {
        OmSegmentDataV3* rawV3 = dataV3.get();

        boost::shared_ptr<OmSegmentDataV4> ret =
            OmSmartPtr<OmSegmentDataV4>::MallocNumElements(pageSize, om::ZERO_FILL);
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

    boost::shared_ptr<uint8_t>
    static ConvertPageV3toV4ListType(boost::shared_ptr<OmSegmentDataV3> dataV3,
                                     const uint32_t pageSize)
    {
        OmSegmentDataV3* rawV3 = dataV3.get();

        boost::shared_ptr<uint8_t> ret =
            OmSmartPtr<uint8_t>::MallocNumElements(pageSize, om::ZERO_FILL);
        uint8_t* raw = ret.get();

        for(uint32_t i = 0; i < pageSize; ++i)
        {
            raw[i] = rawV3[i].listType;
        }

        return ret;
    }
};

#endif
