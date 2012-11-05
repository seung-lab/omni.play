#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"
#include "pointsInCircle.hpp"

#include <set>

namespace om {
namespace pipeline {

class getSegId : public boost::static_visitor<uint32_t>
{
private:
    coords::Data coord_;

public:
    getSegId(coords::Data coord)
        : coord_(coord)
    {}

    template<typename T>
    uint32_t operator()(const datalayer::memMappedFile<T>& in) const
    {
        uint64_t offset = coord_.ToChunk().PtrOffset(coord_.volume(), sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);

        return chunkPtr[coord_.ToChunkOffset()];
    }
};

uint32_t operator>>(const dataSrcs& d, const getSegId& v) {
    return boost::apply_visitor(v, d);
}

class getSegIds : public stage
{
private:
    coords::Data coord_;
    int radius_;
    om::common::ViewType view_;
    coords::DataBbox bounds_;
    static const utility::pointsInCircle pts;

public:
    getSegIds(coords::Data coord, int radius,
              om::common::ViewType view, coords::DataBbox bounds)
        : coord_(coord)
        , radius_(radius)
        , view_(view)
        , bounds_(bounds)
    {}

    template<typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const {
        return getCircle(in);
    }

private:
    template<typename T>
    inline data_var getCircle(const datalayer::memMappedFile<T>& in) const
    {
        const std::vector<Vector2i>& points = pts.GetPtsInCircle(radius_);

        std::set<uint32_t> segments;

        const coords::Data twisted = common::twist(coord_, view_);
        FOR_EACH(it, points)
        {
            coords::Data offseted = twisted;
            offseted.x += it->x;
            offseted.y += it->y;
            coords::Data untwisted = common::twist(offseted, view_);
            if(!bounds_.contains(untwisted)) {
                continue;
            }

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
                             const coords::Data& c) const
    {
        uint64_t offset = c.ToChunk().PtrOffset(c.volume(), sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);

        return chunkPtr[c.ToChunkOffset()];
    }
};

data_var operator>>(const dataSrcs& d, const getSegIds& v) {
    return boost::apply_visitor(v, d);
}

const utility::pointsInCircle getSegIds::pts;

}
}
