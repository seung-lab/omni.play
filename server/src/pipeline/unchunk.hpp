#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"

namespace om {
namespace pipeline {

class unchunk : public stage
{
private:
    const coords::volumeSystem& vs_;

public:
    unchunk(const coords::volumeSystem& vs)
        : vs_(vs)
    { }

    size_t target_offset(coords::data d) const
    {
    	const Vector3i dims = vs_.GetChunkDimensions();
    	return d.x + d.y * dims.x + d.z * dims.x * dims.y;
    }

    template <typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        out.size = vs_.GetDataDimensions().dot(Vector3i::ONE);
		out.data = utility::smartPtr<T>::MallocNumElements(out.size);

		const Vector3i dims = vs_.GetChunkDimensions();

        // iterate over all chunks in order
	    FOR_EACH(iter, *vs_.GetMipChunkCoords(0))
	    {
            coords::chunk coord = *iter;
			coords::data base = coord.toData(&vs_);

            uint64_t offset = coord.chunkPtrOffset(&vs_, sizeof(T));
	        T* chunkPtr = in.GetPtrWithOffset(offset);

            for (int x = 0; x < dims.x; ++x) {
            	for (int y = 0; y < dims.y; ++y) {
            		for (int z = 0; z < dims.z; ++z) {
            			coords::data d(base.x + x, base.y + y, base.z + z, &vs_, 0);
            			out.data.get()[target_offset(d)] = chunkPtr[d.toChunkOffset()];
            		}
            	}
            }

	        std::copy(chunkPtr, &chunkPtr[out.size], out.data.get());
	    }

        return out;
    }
};

data_var operator>>(const dataSrcs& d, const unchunk& v) {
	boost::apply_visitor(v, d);
}

}}
