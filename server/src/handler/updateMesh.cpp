
#include "common/string.hpp"
#include "handler/handler.h"

#include "pipeline/unchunk.hpp"
#include "pipeline/filter.hpp"
#include "volume/volume.h"
#include "RealTimeMesher.h"

namespace om {
namespace handler {

void update_global_mesh(zi::mesh::RealTimeMesherIf* rtm,
                        const volume::volume& vol,
	                    const std::set<uint32_t>& segIds,
                        uint32_t segId)
{
	using namespace pipeline;
	using namespace apache::thrift;
	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	datalayer::memMappedFile<uint32_t> dataFile =
		boost::get<datalayer::memMappedFile<uint32_t> >(vol.Data());
	data<uint32_t> unchunked = unchunk(vol.CoordSystem())(dataFile);
	data<uint32_t> filtered = set_filter<uint32_t>(segIds, segId)(unchunked);

	zi::mesh::Vector3i loc,size;
	loc.x = vol.Bounds().getMin().x + 1;
	loc.y = vol.Bounds().getMin().y + 1;
	loc.z = vol.Bounds().getMin().z + 1;
	size.x = vol.Bounds().getMax().x - vol.Bounds().getMin().x - 1;
	size.y = vol.Bounds().getMax().y - vol.Bounds().getMin().y - 1;
	size.z = vol.Bounds().getMax().z - vol.Bounds().getMin().z - 1;
	std::string data(reinterpret_cast<char*>(filtered.data.get()), filtered.size * sizeof(uint32_t));
	// Integrate with realtime mesher.
	try {
		rtm->remesh(false);
		rtm->update(string::num(segId), loc, size, data);
	} catch (apache::thrift::TException &tx) {
		std::cout << "Something's Wrong: " << tx.what() << std::endl;
	    throw(tx);
	}
}

}} // namespace om::handler::