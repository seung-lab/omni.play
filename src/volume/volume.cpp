
#include "volume/volume.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/getSegIds.hpp"

namespace om {
namespace volume {

using namespace pipeline;

volume::volume(const server::metadata& meta)
    : uri_(meta.uri)
    , bounds_(meta.bounds)
    , resolution_(common::Convert(meta.resolution))
    , type_(meta.type)
    , chunkDims_(common::Convert(meta.chunkDims))
    , mipLevel_(meta.mipLevel)
    , coordSystem_(meta)
{
    const std::string chanName = str(
        boost::format("%1%/channels/channel1/%2%/volume.float.raw")
        % uri_ % mipLevel_);

    mapData mapped(chanName, server::dataType::FLOAT); // Ignore dataType for now.
    chanData_ = mapped.file();

    const std::string segName = str(
        boost::format("%1%/segmentations/segmentation1/%2%/volume.uint32_t.raw")
        % uri_ % mipLevel_);

    mapData mappedSeg(segName, server::dataType::UINT32); // Ignore dataType for now.
    segmentationData_ = mappedSeg.file();
}

int32_t volume::GetSegId(coords::global point) const
{
    coords::global coord = point;
    coords::data dc = coord.toData(&coordSystem_, mipLevel_);

    return segmentationData_ >> getSegId(dc);
}

void volume::GetSegIds(coords::global point, int radius,
                       common::viewType view,
                       std::set<int32_t>& ids) const
{
    coords::global coord = point;
    coords::data dc = coord.toData(&coordSystem_, mipLevel_);

    data_var id = segmentationData_ >> getSegIds(dc, radius, view,
                                                 bounds_.toDataBbox(&coordSystem_, mipLevel_));

    data<uint32_t> found = boost::get<data<uint32_t> >(id);
    for(int i = 0; i < found.size; i++) {
        ids.insert(found.data.get()[i]);
    }
}

segment::data volume::GetSegmentData(int32_t segId) const
{
    if (segId <= 0) {
        throw argException("Not allowed segment Ids less than or equal to 0");
    }

    const uint32_t pageSize = 100000;
    const uint32_t pageNum = segId / pageSize;
    const uint32_t idx = segId % pageSize;
    const std::string fname = str(
        boost::format("%1%/users/_default/segmentations/segmentation1/segments/segment_page%2%.data.ver4")
        % uri_ % pageNum);

    if(!file::exists(fname)) {
        throw argException(str(boost::format("Invalid Seg Id %") % segId));
    }

    datalayer::memMappedFile<segment::data> page(fname);

    segment::data d = page.GetPtr()[idx];

    if(d.value <= 0) {
        throw argException(str(boost::format("Invalid Seg Id %") % segId));
    }

    return d;
}

}} // namespace om::volume::
