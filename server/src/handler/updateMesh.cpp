#include "handler/handler.h"

#include "pipeline/filter.hpp"
#include "volume/volume.h"

namespace om {
namespace handler {

void update_global_mesh(const volume::volume& vol,
	                    const std::set<uint32_t>& segIds,
                        uint32_t segId)
{
	using namespace pipeline;

	if (!vol.VolumeType() == server::volType::SEGMENTATION) {
		throw argException("Can only update global mesh from segmentation");
	}

	data_var filtered = vol.Data() >> set_filter<uint32_t>(segIds, segId);

	data<uint32_t> out = boost::get<data<uint32_t> >(filtered);

	// Integrate with realtime mesher.
}

}} // namespace om::handler::