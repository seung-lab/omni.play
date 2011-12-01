#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"
#include "pointsInCircle.hpp"

#include <set>

namespace om {
namespace pipeline {

class getSegIds : public stage
{
private:
    coords::data coord_;
    int radius_;
    server::viewType::type view_;
    static const utility::pointsInCircle pts;

public:
    getSegIds(coords::data coord)
        : coord_(coord)
        , radius_(0)
    {}

    getSegIds(coords::data coord, int radius, server::viewType::type view)
        : coord_(coord)
        , radius_(radius)
        , view_(view)
    {}

    template<typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        if(radius_ == 0) {
            return getSingleSeg(in);
        } else { // brush select
            return getCircle(in);
        }

        return data<uint32_t>();
    }

private:
    template<typename T>
    inline data_var getSingleSeg(const datalayer::memMappedFile<T>& in) const
    {
        data<uint32_t> segs;
        segs.data = utility::smartPtr<uint32_t>::MallocNumElements(1);
        segs.size = 1;

        segs.data.get()[0] = getSegId(in, coord_);

        return segs;
    }

    template<typename T>
    inline data_var getCircle(const datalayer::memMappedFile<T>& in) const
    {
        const std::vector<Vector2i>& points = pts.GetPtsInCircle(radius_);

        std::set<uint32_t> segments;

        const coords::data twisted = common::twist(coord_, view_);
        FOR_EACH(it, points)
        {
            coords::data offseted = twisted;
            offseted.x += it->x;
            offseted.y += it->y;
            coords::data untwisted = common::twist(offseted, view_);
            int32_t segId = getSegId(in, untwisted);
            segments.insert(segId);
        }

        data<uint32_t> segs;
        segs.data = utility::smartPtr<uint32_t>::MallocNumElements(segments.size());
        segs.size = segments.size();

        std::copy(segments.begin(), segments.end(), segs.data.get());

        return segs;
    }

    template<typename T>
    inline uint32_t getSegId(const datalayer::memMappedFile<T>& in,
                             const coords::data& c) const
    {
        uint64_t offset = c.toChunk().chunkPtrOffset(c.volume(), sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);

        return chunkPtr[c.toChunkOffset()];
    }
};

data_var operator>>(const dataSrcs& d, const getSegIds& v) {
    return boost::apply_visitor(v, d);
}

const utility::pointsInCircle getSegIds::pts;

}
}
