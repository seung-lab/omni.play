
#include "common/string.hpp"
#include "handler/handler.h"

#include "pipeline/unchunk.hpp"
#include "pipeline/filter.hpp"
#include "volume/volume.h"
#include "RealTimeMesher.h"

#include <fstream>

namespace om {
namespace handler {

template<typename T>
void write_out(T* data, size_t size, std::string fnp)
{
	std::ofstream out(fnp.c_str(), std::ios_base::out | std::ios_base::binary);
	if(!out.good()) {
		std::cout << "Unable to open file: " << fnp << std::endl;
	}
	out.write(reinterpret_cast<char*>(data), size * sizeof(T));
	out.flush();
	out.close();
}

void make_loc_size(const volume::volume& vol, zi::mesh::Vector3i& loc, zi::mesh::Vector3i& size)
{
	loc.x = vol.Bounds().getMin().x + 1;
	loc.y = vol.Bounds().getMin().y + 1;
	loc.z = vol.Bounds().getMin().z + 1;
	size.x = vol.Bounds().getMax().x - vol.Bounds().getMin().x;
	size.y = vol.Bounds().getMax().y - vol.Bounds().getMin().y;
	size.z = vol.Bounds().getMax().z - vol.Bounds().getMin().z;
}

bool modify_global_mesh_data(zi::mesh::RealTimeMesherIf* rtm,
                             const volume::volume& vol,
    					     const std::set<uint32_t> addedSegIds,
    					     const std::set<uint32_t> modifiedSegIds,
    					     int32_t segId)
{
	using namespace pipeline;
	using namespace apache::thrift;

	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	datalayer::memMappedFile<uint32_t> dataFile =
		boost::get<datalayer::memMappedFile<uint32_t> >(vol.Data(0));

	data<uint32_t> unchunked = unchunk(vol.CoordSystem())(dataFile);

	data<uint32_t> addedFiltered = set_filter<uint32_t, uint32_t>(addedSegIds, segId)(unchunked);

	data<char> modifiedFiltered = set_filter<uint32_t, char>(modifiedSegIds, 1)(unchunked);

	std::string data(reinterpret_cast<char*>(addedFiltered.data.get()), addedFiltered.size * sizeof(uint32_t));
	std::string mask(modifiedFiltered.data.get(), modifiedFiltered.size);

	zi::mesh::Vector3i loc, size;
	make_loc_size(vol, loc, size);

	try {
		rtm->maskedUpdate(string::num(segId), loc, size, data, mask);
	} catch (apache::thrift::TException &tx) {
		std::cout << "Something's Wrong: " << tx.what() << std::endl;
	    return false;
	}
	return true;
}

}} // namespace om::handler::