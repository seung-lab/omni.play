
#include "volume/volume.h"
#include "datalayer/memMappedFile.hpp"
#include "utility/timer.hpp"
#include "segment/segmentTypes.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/getSegIds.hpp"
#include <zi/zlog/zlog.hpp>
#include <boost/optional.hpp>

#include "utility/yaml/baseTypes.hpp"
#include "utility/yaml/yaml.hpp"

namespace om {
namespace volume {

using namespace pipeline;

bool volume::loadVolume(const int32_t mipLevel)
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
                % uri_ % mipLevel);

            if(!file::exists(chanName)){
        		return false;
        	}

            mapData mapped(chanName, server::dataType::FLOAT); // Ignore dataType for now.
            data_.push_back(mapped.file());
            break;
        }
        case server::volType::SEGMENTATION:
        {
            const std::string segName = str(
                boost::format("%1%/segmentations/segmentation1/%2%/volume.uint32_t.raw")
                % uri_ % mipLevel);

			if(!file::exists(segName)){
        		return false;
        	}

            mapData mappedSeg(segName, server::dataType::UINT32); // Ignore dataType for now.
            data_.push_back(mappedSeg.file());
            break;
        }
    }
    return true;
}

void volume::loadVolume()
{
	for (int i = 0;; ++i)
	{
		if(!loadVolume(i))
			break;
	}
}

volume::volume(const server::metadata& meta)
    : uri_(meta.uri)
    , bounds_(meta.bounds)
    , resolution_(common::Convert(meta.resolution))
    , dataType_(meta.type)
    , volType_(meta.vol_type)
    , chunkDims_(common::Convert(meta.chunkDims))
    , coordSystem_(meta)
{
    loadVolume();
}

volume::volume(std::string uri,
    	   	   coords::globalBbox bounds,
    	   	   Vector3i resolution,
    	   	   server::dataType::type dataType,
    	   	   server::volType::type volType,
    	   	   Vector3i chunkDims)
    	: uri_(uri)
    	, bounds_(bounds)
    	, resolution_(resolution)
    	, dataType_(dataType)
    	, volType_(volType)
    	, chunkDims_(chunkDims)
    	, coordSystem_()
{
	Vector3i dims = bounds.getMax() - bounds.getMin();
	coordSystem_.SetDataDimensions(dims);
    coordSystem_.SetAbsOffset(bounds.getMin());
    coordSystem_.SetResolution(resolution);
    coordSystem_.UpdateRootLevel();
	loadVolume();
}

void volume::loadYaml(const YAML::Node& yamlVolume)
{
	const YAML::Node& coords = yamlVolume["coords"];

	coords::global dims, absOffset;
	coords["dataDimensions"] >> dims;
	coords["absOffset"] >> absOffset;
	coords["dataResolution"] >> resolution_;

	bounds_ = coords::globalBbox(absOffset, absOffset + dims);

	std::string type;
	yamlVolume["type"] >> type;
	if(type == "float") {
		dataType_ = server::dataType::FLOAT;
	} else if (type == "uint32_t") {
		dataType_ = server::dataType::UINT32;
	}

	int chunkDim;
	coords["chunkDim"] >> chunkDim;
	chunkDims_ = Vector3i(chunkDim);
}

volume::volume(std::string uri, common::objectType type)
    	: uri_(uri)
{
	YAML::Node n;
	std::string fnp = uri + "/projectMetadata.yaml";
	if(!boost::filesystem::exists(fnp)){
        throw ioException("could not find file", fnp);
    }

    std::ifstream fin(fnp.c_str());

    YAML::Parser parser(fin);

    parser.GetNextDocument(n);
    parser.GetNextDocument(n);

	const YAML::Node& volumes = n["Volumes"];

	if(type == common::CHANNEL) {
		volType_ = server::volType::CHANNEL;
		loadYaml(volumes["Channels"]["values"][0]);
	} else {
		volType_ = server::volType::SEGMENTATION;
		loadYaml(volumes["Segmentations"]["values"][0]);
	}

	Vector3i dims = bounds_.getMax() - bounds_.getMin();
	coordSystem_.SetDataDimensions(dims);
    coordSystem_.SetAbsOffset(bounds_.getMin());
    coordSystem_.SetResolution(resolution_);
    coordSystem_.UpdateRootLevel();
	loadVolume();
}


int32_t volume::GetSegId(coords::global point) const
{
    coords::global coord = point;
    coords::data dc = coord.toData(&coordSystem_, 0);

    return data_[0] >> getSegId(dc);
}

boost::optional<segments::data> volume::GetSegmentData(int32_t segId) const
{
    if (segId <= 0) {
        return false;
    }

    const uint32_t pageSize = 100000;
    const uint32_t pageNum = segId / pageSize;
    const uint32_t idx = segId % pageSize;
    const std::string fname = str(
        boost::format("%1%/users/_default/segmentations/segmentation1/segments/segment_page%2%.data.ver4")
        % uri_ % pageNum);

    if(!file::exists(fname)) {
        return false;
    }

    datalayer::memMappedFile<segments::data> page(fname);

    if(segId >= page.Length()) {
    	return false;
    }

    segments::data d = page.GetPtr()[idx];

    if(d.value <= 0) {
        return false;
    }

    return d;
}

}} // namespace om::volume::
