#pragma once

#include "datalayer/types.hpp"
#include "datalayer/cachedType.hpp"

namespace om {
namespace segment {

class page : datalayer::cachedType<datalayer::SegmentationData>
{
private:
    typedef datalayer::cachedType<datalayer::SegmentationData> base_t;
public:
    static const int PAGE_SIZE;

    page() : base_t() {}
    page(char* data, size_t size);

    inline const segment::data& operator[](uint i) const
    {
        if(i >= PAGE_SIZE) {
            throw argException("SegData index out of bounds.");
        }

        return data_->Data[i];
    }

    inline segment::data& operator[](uint i) {
        if(i >= PAGE_SIZE) {
            throw argException("SegData index out of bounds.");
        }

        return data_->Data[i];
    }

    static std::string GetKey(int volumeId, uint8_t pageNum)
    {
        return str(boost::format("%1%(SegData):%2%") % volumeId % pageNum);
    }
};

}} // namespace om::segment::
