#include "handler/handler.h"

#include "pipeline/filter.hpp"
#include "volume/volume.h"
#include "RealTimeMesher.h"
#include "serverHandler.hpp"

namespace om {
namespace handler {

void update_global_mesh(const server::serverHandler* const handler,
                        const volume::volume& vol,
	                    const std::set<uint32_t>& segIds,
                        uint32_t segId)
{
	using namespace pipeline;

	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	data_var filtered = vol.Data() >> set_filter<uint32_t>(segIds, segId);

	data<uint32_t> out = boost::get<data<uint32_t> >(filtered);

	zi::mesh::Vector3i loc,size;
	loc.x = vol.Bounds().getMin().x;
	loc.y = vol.Bounds().getMin().y;
	loc.z = vol.Bounds().getMin().z;
	size.x = vol.Bounds().getMax().x - vol.Bounds().getMin().x;
	size.y = vol.Bounds().getMax().y - vol.Bounds().getMin().y;
	size.z = vol.Bounds().getMax().z - vol.Bounds().getMin().z;
	std::string data(reinterpret_cast<char*>(out.data.get()), out.size);
	// Integrate with realtime mesher.
	handler->mesher()->queueUpdate(vol.Uri(), loc, size, data);

	handler->mesher()->remesh(false);
}

}} // namespace om::handler::