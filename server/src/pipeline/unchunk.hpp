#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "utility/smartPtr.hpp"
#include <boost/multi_array.hpp>

namespace om {
namespace pipeline {

class unchunk //: public stage
{
private:
    const coords::volumeSystem& vs_;
	Vector3i dims_;
	const Vector3i chunkDims_;
	const Vector3i mipDims_;

public:
    unchunk(const coords::volumeSystem& vs)
        : vs_(vs)
        , dims_(vs_.GetDataDimensions())
        , chunkDims_(vs_.GetChunkDimensions())
        , mipDims_(vs_.MipLevelDimensionsInMipChunks(0))
    {
    	dims_.x -= 2;
    	dims_.y -= 2;
    	dims_.z -= 2;
    }

    inline size_t target_offset(coords::data d) const
    {
    	return d.x + d.y * dims_.x + d.z * dims_.x * dims_.y;
    }

    size_t getFrom(int chunkIdx, int dim) const
    {
    	if (chunkIdx == 0) {
    		return 1;
    	} else {
    		return chunkIdx * chunkDims_[dim];
    	}
    }

    size_t getTo(int chunkIdx, int dim) const
    {
    	size_t to = chunkIdx * chunkDims_[dim];
    	if (chunkIdx == mipDims_[dim] - 1) {
    		return to - 1;
    	} else {
    		return to;
    	}
    }

    template <typename T>
    data<T> operator()(const datalayer::memMappedFile<T>& in) const
    {
    	using namespace boost;
    	typedef multi_array_ref<T, 3> array_type;
    	typedef const_multi_array_ref<T, 3> const_array_type;
    	typedef typename const_array_type::template array_view<3>::type const_view_type;
    	typedef boost::multi_array_types::index_range range;

		array<size_t, 3> volExent = {{dims_.x, dims_.y, dims_.z}};
		array<size_t, 3> chunkExent = {{chunkDims_.x, chunkDims_.y, chunkDims_.z}};

    	data<T> out;
        out.size = dims_.x * dims_.y * dims_.z;
		out.data = utility::smartPtr<T>::MallocNumElements(out.size);
		array_type volData(out.data.get(), volExent, fortran_storage_order());
		T* outPtr = out.data.get();

        shared_ptr<std::deque<coords::chunk> > chunks = vs_.GetMipChunkCoords(0);
	    FOR_EACH(iter, *chunks)
	    {
	    	const coords::chunk& coord = *iter;
	    	coords::data base = coord.toData(&vs_);

            uint64_t offset = coord.chunkPtrOffset(&vs_, sizeof(T));
            array_type chunkData(in.GetPtrWithOffset(offset), chunkExent, fortran_storage_order());
            const_view_type view = chunkData[ indices[range(getFrom(coord.X(), 0),getTo(coord.X(), 0))]
                               				  		 [range(getFrom(coord.Y(), 1),getTo(coord.Y(), 1))]
                               				  		 [range(getFrom(coord.Z(), 2),getTo(coord.Z(), 2))]];


            for (int z = 0; z < view.shape()[2] && z < dims_.z; ++z) {
            	for (int y = 0; y < view.shape()[1] && y < dims_.y; ++y) {
            		coords::data d(base.x, base.y + y, base.z + z, &vs_, 0);

            		size_t width = view.shape()[0];
            		if (dims_.x % chunkDims_.x < width) {
            			width = dims_.x % chunkDims_.x;
            		}
            		std::copy(&view[d.x][d.y][d.z], &view[d.x + width][d.y][d.z], &volData[d.x][d.y][d.z]);
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
