
#include "common/string.hpp"
#include "handler/handler.h"

#include "pipeline/unchunk.hpp"
#include "pipeline/filter.hpp"
#include "volume/volume.h"
#include "RealTimeMesher.h"

#include "boost/multi_array.hpp"

#include <fstream>
#include <algorithm>

namespace om {
namespace handler {

class UpdateRTM : public boost::static_visitor<>
{
public:
	UpdateRTM(const coords::volumeSystem& vs,
	          zi::mesh::RealTimeMesherIf* rtm,
			  const std::set<uint32_t> addedSegIds,
			  const std::set<uint32_t> modifiedSegIds,
			  int32_t segId)
		: vs_(vs)
		, rtm_(rtm)
		, addedSegIds_(addedSegIds)
		, modifiedSegIds_(modifiedSegIds)
		, segId_(segId)
	{}

	template<typename T>
	void operator()(datalayer::memMappedFile<T> in) const
	{
		using namespace apache::thrift;
		using namespace boost;

		typedef boost::multi_array_ref<T, 3> array;
		typedef typename array::template array_view<3>::type array_view;
		typedef boost::multi_array_types::index_range range;

		shared_ptr<std::deque<coords::chunk> > chunks = vs_.GetMipChunkCoords(0);
	    FOR_EACH(iter, *chunks)
	    {
	    	const coords::chunk& cc = *iter;

	    	uint64_t offset = cc.chunkPtrOffset(&vs_, sizeof(T));
	        T* chunkPtr = in.GetPtrWithOffset(offset);

	        array chunkData(chunkPtr, extents[128][128][128]);

	        coords::dataBbox bounds = cc.chunkBoundingBox(&vs_);
	        coords::data min = bounds.getMin();
	        if (min.x == 0) {
	        	min.x += TRIM;
	        }

	        if (min.y == 0) {
	        	min.y += TRIM;
	        }

	        if (min.z == 0) {
	        	min.z += TRIM;
	        }

			coords::data max = bounds.getMax();
			Vector3i volMax = vs_.GetDataDimensions();
			if (max.x >= volMax.x) {
	        	max.x = volMax.x - TRIM;
	        }

	        if (max.y >= volMax.y) {
	        	max.y = volMax.y - TRIM;
	        }

	        if (max.z >= volMax.z) {
	        	max.z = volMax.z - TRIM;
	        }

	        if(max.x <= min.x || max.y <= min.y || max.z <= min.z) {
	        	continue;
	        }

	        typename array::index_gen indices;
	        array_view regionOfInterest = chunkData[indices[range(min.x,max.x)]
	                                                	   [range(min.y,max.y)]
	                                                	   [range(min.z,max.z)]];

	        send<T>(regionOfInterest, min.toGlobal());
	    }
	}

private:
	const coords::volumeSystem& vs_;
	zi::mesh::RealTimeMesherIf* rtm_;
	const std::set<uint32_t> addedSegIds_;
	const std::set<uint32_t> modifiedSegIds_;
	int32_t segId_;
	static const int TRIM = 3;

	template <typename T, typename Arr>
	void send(const Arr& data, coords::global location) const
	{
		using namespace boost;

		const typename Arr::size_type* shape = data.shape();
		size_t length = shape[0] * shape[1] * shape[2];

		std::string out(length * sizeof(T), 0);
		multi_array_ref<T, 3> out_ref(reinterpret_cast<T*>(const_cast<char*>(out.data())), extents[shape[0]][shape[1]][shape[2]]);

		std::string mask(length, 0);
		multi_array_ref<char, 3> mask_ref(const_cast<char*>(mask.data()), extents[shape[0]][shape[1]][shape[2]]);

		for (int x = 0; x < shape[0]; ++x) {
			for (int y = 0; y < shape[1]; ++y) {
				for (int z = 0; z < shape[2]; ++z)
				{
					if(addedSegIds_.count(data[x][y][z])) {
						out_ref[x][y][z] = segId_;
					}
					if(modifiedSegIds_.count(data[x][y][z])) {
						mask_ref[x][y][z] = 1;
					}
				}
			}
		}

		zi::mesh::Vector3i loc, size;
		loc.x = location.x; loc.y = location.y; loc.z = location.z;
		size.x = shape[0]; size.y = shape[1]; size.z = shape[2];

		bool succeded = false;
		do
		{
			try
			{
				rtm_->maskedUpdate(string::num(segId_), loc, size, out, mask);
				succeded = true;
			}
			catch (apache::thrift::TException &tx)
			{
				std::cout << "Unable to update RTM: " << tx.what() << std::endl;
				sleep(1000);
			}
		} while(!succeded);
	}
};

bool modify_global_mesh_data(zi::mesh::RealTimeMesherIf* rtm,
                             const volume::volume& vol,
    					     const std::set<uint32_t> addedSegIds,
    					     const std::set<uint32_t> modifiedSegIds,
    					     int32_t segId)
{
	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	boost::apply_visitor(UpdateRTM(vol.CoordSystem(), rtm, addedSegIds, modifiedSegIds, segId), vol.Data(0));
}

}} // namespace om::handler::