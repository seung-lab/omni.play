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
	coords::dataBbox dataBounds_;
	const Vector3i chunkDims_;
	Vector3i dims_;

public:
    unchunk(const coords::volumeSystem& vs)
        : vs_(vs)
        , dataBounds_(vs_.GetDataExtent().toDataBbox(&vs_, 0))
        , chunkDims_(vs_.GetChunkDimensions())
    {
    	dataBounds_.setMin(dataBounds_.getMin() + Vector3i::ONE);
    	dataBounds_.setMax(dataBounds_.getMax() - Vector3i::ONE);
    	dims_ = dataBounds_.getDimensions();
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

        shared_ptr<std::deque<coords::chunk> > chunks = vs_.GetMipChunkCoords(0);
	    FOR_EACH(iter, *chunks)
	    {
	    	const coords::chunk& coord = *iter;
    		coords::dataBbox chunkBounds = coord.chunkBoundingBox(&vs_);
    		chunkBounds.intersect(dataBounds_); // crop to volume.

    		// Shift min and max to chunk space.
    		Vector3i min = chunkBounds.getMin() % chunkDims_;
    		Vector3i max = (chunkBounds.getMax() - Vector3i::ONE )% chunkDims_ + Vector3i::ONE; // bounds are exclusive
    		const size_t width = max.x - min.x - 1;

            uint64_t offset = coord.chunkPtrOffset(&vs_, sizeof(T));
            array_type chunkData(in.GetPtrWithOffset(offset), chunkExent, fortran_storage_order());
            const_view_type view = chunkData[ indices[range(min.x, max.x)][range(min.y, max.y)][range(min.z, max.z)]];

            Vector3i base = chunkBounds.getMin() - Vector3i::ONE; // offset the output 1 for the boundry 0s

            for (int x = 0; x < view.shape()[0] && x < dims_.x; ++x) {
            	for (int y = 0; y < view.shape()[1] && y < dims_.y; ++y) {
            		for (int z = 0; z < view.shape()[2] && z < dims_.z; ++z) {
            			volData[base.x + x][base.y + y][base.z + z] = view[x][y][z];
            		}
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
