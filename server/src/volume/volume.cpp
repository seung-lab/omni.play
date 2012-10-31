
#include "volume/volume.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/getSegIds.hpp"
#include <zi/zlog/zlog.hpp>

namespace om {
namespace volume {

using namespace pipeline;

void volume::loadVolume()
{
	if(!file::exists(uri_)) {
        throw argException("Invalid metadata: uri not found.");
    }

    switch(volType_)
    {
        case server::volType::CHANNEL:
        {
            const std::string chanName = str(
                boost::format("%1%/channels/channel1/%2%/volume.float.raw")
                % uri_ % mipLevel_);

            mapData mapped(chanName, server::dataType::FLOAT); // Ignore dataType for now.
            data_ = mapped.file();
            break;
        }
        case server::volType::SEGMENTATION:
        {
            const std::string segName = str(
                boost::format("%1%/segmentations/segmentation1/%2%/volume.uint32_t.raw")
                % uri_ % mipLevel_);

            mapData mappedSeg(segName, server::dataType::UINT32); // Ignore dataType for now.
            data_ = mappedSeg.file();
            break;
        }
    }
}

volume::volume(const server::metadata& meta)
    : uri_(meta.uri)
    , bounds_(meta.bounds)
    , resolution_(common::Convert(meta.resolution))
    , dataType_(meta.type)
    , volType_(meta.vol_type)
    , chunkDims_(common::Convert(meta.chunkDims))
    , mipLevel_(meta.mipLevel)
    , coordSystem_(meta)
{
    loadVolume();
}

volume::volume(std::string uri,
    	   	   coords::GlobalBbox bounds,
    	   	   Vector3i resolution,
    	   	   server::dataType::type dataType,
    	   	   server::volType::type volType,
    	   	   Vector3i chunkDims,
    	   	   int32_t mipLevel)
    	: uri_(uri)
    	, bounds_(bounds)
    	, resolution_(resolution)
    	, dataType_(dataType)
    	, volType_(volType)
    	, chunkDims_(chunkDims)
    	, mipLevel_(mipLevel)
    	, coordSystem_()
{
	Vector3i dims = bounds.getMax() - bounds.getMin();
	coordSystem_.SetDataDimensions(dims);
    coordSystem_.SetAbsOffset(bounds.getMin());
    coordSystem_.SetResolution(resolution);
    coordSystem_.UpdateRootLevel();
	loadVolume();
}

int32_t volume::GetSegId(coords::Global point) const
{
    coords::Global coord = point;
    coords::Data dc = coord.ToData(&coordSystem_, mipLevel_);

    return data_ >> getSegId(dc);
}

void volume::GetSegIds(coords::Global point, int radius,
                       common::viewType view,
                       std::set<int32_t>& ids) const
{
    coords::Global coord = point;
    coords::Data dc = coord.ToData(&coordSystem_, mipLevel_);

    data_var id = data_ >> getSegIds(dc, radius, view,
                                                 bounds_.ToDataBbox(&coordSystem_, mipLevel_));

    data<uint32_t> found = boost::get<data<uint32_t> >(id);
    for(int i = 0; i < found.size; i++) {
        ids.insert(found.data.get()[i]);
    }
}

segments::data volume::GetSegmentData(int32_t segId) const
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
        throw argException(str(boost::format("Invalid Seg Id %1%") % segId));
    }

    datalayer::memMappedFile<segments::data> page(fname);

    segments::data d = page.GetPtr()[idx];

    if(d.value <= 0) {
        throw argException(str(boost::format("Invalid Seg Id %1%") % segId));
    }

    return d;
}

}} // namespace om::volume::
