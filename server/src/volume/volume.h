#pragma once

#include "server_types.h"
#include "common/common.h"
#include "pipeline/mapData.hpp"
#include "segment/segmentTypes.hpp"

namespace om {
namespace volume {

class volume
{
private:
    std::string uri_;
    coords::globalBbox bounds_;
    Vector3i resolution_;
    server::dataType::type dataType_; // Replace with omni type?
    server::volType::type volType_;
    Vector3i chunkDims_;
    std::vector<pipeline::dataSrcs> data_;
//    segments::dataManager segData_;
    coords::volumeSystem coordSystem_;

    friend std::ostream& operator<<(std::ostream& out, const volume& vol);

public:
    volume(const server::metadata& metadata);

    volume(std::string uri,
    	   coords::globalBbox bounds,
    	   Vector3i resolution,
    	   server::dataType::type dataType,
    	   server::volType::type volType,
    	   Vector3i chunkDims);

    volume(std::string uri, common::objectType type);

    inline const std::string& Uri() const {
        return uri_;
    }

    inline const coords::globalBbox& Bounds() const {
        return bounds_;
    }

    inline const Vector3i& Resolution() const {
        return resolution_;
    }

    inline server::dataType::type DataType() const {
        return dataType_;
    }

    inline server::volType::type VolumeType() const {
        return volType_;
    }

    inline const Vector3i& ChunkDims() const {
        return chunkDims_;
    }

    inline const pipeline::dataSrcs& Data(int32_t mipLevel) const {
        return data_[mipLevel];
    }

    inline const coords::volumeSystem& CoordSystem() const {
        return coordSystem_;
    }

    int32_t GetSegId(coords::global point) const;

    boost::optional<segments::data> GetSegmentData(int32_t segId) const;

private:
	bool loadVolume(const int32_t mipLevel);
	void loadVolume();
	void loadYaml(const YAML::Node&);
};

inline std::ostream& operator<<(std::ostream& out, const volume& vol)
{
	out << "{" << std::endl
		<< "\turi:\t" << vol.uri_ << std::endl
    	<< "\tbounds:\t" << vol.bounds_ << std::endl
    	<< "\tresolution:\t" << vol.resolution_ << std::endl
    	<< "\tdataType:\t" << vol.dataType_ << std::endl
    	<< "\tvolType:\t" << vol.volType_ << std::endl
    	<< "\tchunkDims:\t" << vol.chunkDims_ << std::endl
    	<< "}";
	return out;
}

}} // namespace om::volume::
