#pragma once

#include "common/common.h"
#include "pipeline/mapData.hpp"
#include "segment/segmentTypes.hpp"

namespace om {
namespace volume {

class volume
{
private:
    std::string uri_;
    coords::GlobalBbox bounds_;
    Vector3i resolution_;
    common::type dataType_; // Replace with omni type?
    common::objectType volType_;
    Vector3i chunkDims_;
    int32_t mipLevel_;
    pipeline::dataSrcs data_;
//    segments::dataManager segData_;
    coords::VolumeSystem coordSystem_;

    friend std::ostream& operator<<(std::ostream& out, const volume& vol);

public:
    volume(const server::metadata& metadata);
    volume(std::string uri,
    	   coords::GlobalBbox bounds,
    	   Vector3i resolution,
    	   server::dataType::type dataType,
    	   server::volType::type volType,
    	   Vector3i chunkDims,
    	   int32_t mipLevel_);

    inline const std::string& Uri() const {
        return uri_;
    }

    inline const coords::GlobalBbox& Bounds() const {
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

    inline int32_t MipLevel() const {
        return mipLevel_;
    }

    inline const pipeline::dataSrcs& Data() const {
        return data_;
    }

    inline const coords::VolumeSystem& CoordSystem() const {
        return coordSystem_;
    }

//    tile::tile GetChanTile(coords::Global point, server::viewType::type view);
//    tile::tile GetSegTile(coords::Global point, server::viewType::type view);
    int32_t GetSegId(coords::Global point) const;

    void GetSegIds(coords::Global point,
                   int radius,
                   common::viewType view,
                   std::set<int32_t>& ret) const;

    segments::data GetSegmentData(int32_t segId) const;

private:
	void loadVolume();
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
    	<< "\tmipLevel:\t" << vol.mipLevel_ << std::endl
    	<< "}";
	return out;
}

}} // namespace om::volume::
