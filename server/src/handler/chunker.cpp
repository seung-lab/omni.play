#include "handler/handler.h"
#include "pipeline/chunk.h"
#include "pipeline/utility.hpp"
#include "volume/volume.h"
#include <sstream>

namespace om {
using namespace pipeline;
namespace handler{
		
void get_chunk(std::string& _return,
			   const volume::volume& vol, 
			   const server::vector3i& chunk)
{
	coords::Chunk chunkCoord(0, chunk.x, chunk.y, chunk.z);
	vol.Data() >> getChunk(vol.CoordSystem(), chunkCoord) >> write_out(_return);
}

}}