
#include "volume/volume.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "segment/page.h"
#include "pipeline/mapData.hpp"
#include "pipeline/getSegIds.hpp"
#include "tiles/tile.h"

namespace om {
namespace volume {

using namespace pipeline;

volume::volume(const server::metadata& meta)
    : id_(meta.volId)
    , channelType_(meta.channelType)
    , segmentationType_(meta.segmentationType)
    , coordSystem_(meta)
{ }

int32_t volume::GetSegId(coords::global point)
{
    chunks::ChunkVar chunk = getChunk(point, chunks::SEGMENTATION, segmentationType_);

    return chunk >> getSegId(point.toData(&coordSystem_, 0));
}

void volume::GetSegIds(coords::global point, int radius,
                       common::viewType view,
                       std::set<int32_t>& ids)
{
    chunks::ChunkVar chunk = getChunk(point, chunks::SEGMENTATION, segmentationType_);

    data_var id = chunk >> getSegIds(point.toData(&coordSystem_, 0), radius, view);

    data<uint32_t> found = boost::get<data<uint32_t> >(id);
    for(int i = 0; i < found.size; i++) {
        ids.insert(found.data.get()[i]);
    }
}

segment::data volume::GetSegmentData(int32_t segId)
{
    if (segId <= 0) {
        throw argException("Not allowed segment Ids less than or equal to 0");
    }

    const uint32_t pageSize = 100000;
    const uint32_t pageNum = segId / pageSize;
    const uint32_t idx = segId % pageSize;

    segment::page* p = cache_.Get<segment::page>(segment::page::GetKey(id_, pageNum));

    if(!p) {
        throw argException(str(boost::format("Invalid segId %.") % segId));
    }

    segment::data d = (*p)[idx];

    if(!d.value) {
        throw argException(str(boost::format("Invalid segId %.") % segId));
    }

    return d;
}


tiles::Tile volume::GetChanTile(coords::global point, common::viewType view)
{
    return tiles::Tile(&coordSystem_,
                       getChunk(point, chunks::CHANNEL, channelType_),
                       point.toData(&coordSystem_, 0),
                       view);
}

tiles::Tile volume::GetSegTile(coords::global point, common::viewType view)
{
    return tiles::Tile(&coordSystem_,
                       getChunk(point, chunks::SEGMENTATION, segmentationType_),
                       point.toData(&coordSystem_, 0),
                       view);
}


}} // namespace om::volume::
