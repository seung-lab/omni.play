#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"

namespace om {
namespace pipeline {

class unchunk //: public stage
{
private:
    const coords::VolumeSystem& vs_;
	const Vector3i dims_;

public:
    unchunk(const coords::VolumeSystem& vs)
        : vs_(vs)
        , dims_(vs_.DataDimensions())
    { }

    inline size_t target_offset(coords::Data d) const
    {
    	return d.x + d.y * dims_.x + d.z * dims_.x * dims_.y;
    }

    template <typename T>
    data<T> operator()(const datalayer::memMappedFile<T>& in) const
    {
    	data<T> out;
        out.size = dims_.x * dims_.y * dims_.z;
		out.data = utility::smartPtr<T>::MallocNumElements(out.size);
		T* outPtr = out.data.get();

		const Vector3i dims = vs_.ChunkDimensions();
        // iterate over all chunks in order
        // TODO: boost multi_array
        boost::shared_ptr<std::vector<coords::Chunk> > chunks = vs_.MipChunkCoords(0);
	    FOR_EACH(iter, *chunks)
	    {
            coords::Chunk coord = *iter;
            coords::Data base = coord.ToData(&vs_);
            uint64_t offset = coord.PtrOffset(&vs_, sizeof(T));
	        T* chunkPtr = in.GetPtrWithOffset(offset);

            for (int z = 0; z < dims.z; ++z) {
            	for (int y = 0; y < dims.y; ++y) {
        			coords::Data d(base.x, base.y + y, base.z + z, &vs_, 0);
        			size_t off = d.ToChunkOffset();
        			std::copy(&chunkPtr[off], &chunkPtr[off + dims.x], &outPtr[target_offset(d)]);
            	}
            }
	    }
	    return out;
    }
};

// data_var operator>>(const dataSrcs& d, const unchunk& v) {
// 	boost::apply_visitor(v, d);
// }

}}
