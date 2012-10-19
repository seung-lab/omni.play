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

	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	datalayer::memMappedFile<uint32_t> dataFile =
		boost::get<datalayer::memMappedFile<uint32_t> >(vol.Data());
	data<uint32_t> unchunked = unchunk(vol.CoordSystem())(dataFile);
	data<uint32_t> filtered = set_filter<uint32_t>(segIds, segId)(unchunked);

	zi::mesh::Vector3i loc,size;
	loc.x = vol.Bounds().getMin().x;
	loc.y = vol.Bounds().getMin().y;
	loc.z = vol.Bounds().getMin().z;
	size.x = vol.Bounds().getMax().x - vol.Bounds().getMin().x;
	size.y = vol.Bounds().getMax().y - vol.Bounds().getMin().y;
	size.z = vol.Bounds().getMax().z - vol.Bounds().getMin().z;
	std::string data(reinterpret_cast<char*>(filtered.data.get()), filtered.size);
	// Integrate with realtime mesher.
	rtm->update(vol.Uri(), loc, size, data);

	rtm->remesh(false);
}

}} // namespace om::handler::