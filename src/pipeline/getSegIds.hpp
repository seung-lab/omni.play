#pragma once

#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

class getSegIds : public stage
{
private:
    coords::dataCoord coord_;
    double radius_;

public:
    getSegIds(coords::dataCoord coord, double radius)
        : coord_(coord)
        , radius_(radius)
    {}

    template<typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<uint32_t> segs;

        if(radius_ == 0) // single seg
        {
            segs.data.reset(new uint32_t[1]);
            segs.size = 1;

            uint64_t offset = coord_.toChunkCoord().chunkPtrOffset(coord_.volume(), sizeof(T));
            T* chunkPtr = in.GetPtrWithOffset(offset);

            segs.data.get()[0] = chunkPtr[coord_.toChunkOffset()];
        } else { // brush select

        }

        return segs;
    }
};

data_var operator>>(const dataSrcs& d, const getSegIds& v) {
    return boost::apply_visitor(v, d);
}


}
}
