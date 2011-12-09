#pragma once

#include "thrift/server_types.h"
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
    server::dataType::type type_; // Replace with omni type?
    Vector3i chunkDims_;
    int32_t mipLevel_;
    pipeline::dataSrcs chanData_;
    pipeline::dataSrcs segmentationData_;
//    segment::dataManager segData_;
    coords::volumeSystem coordSystem_;

public:
    volume(const server::metadata& metadata);

    inline const std::string& Uri() const {
        return uri_;
    }

    inline const coords::globalBbox& Bounds() const {
        return bounds_;
    }

    inline const Vector3i& Resolution() const {
        return resolution_;
    }

    inline server::dataType::type Type() const {
        return type_;
    }

    inline const Vector3i& ChunkDims() const {
        return chunkDims_;
    }

    inline int32_t MipLevel() const {
        return mipLevel_;
    }

    inline const pipeline::dataSrcs& ChannelData() const {
        return chanData_;
    }

    inline const pipeline::dataSrcs& SegmentationData() const {
        return segmentationData_;
    }

    inline const coords::volumeSystem& CoordSystem() const {
        return coordSystem_;
    }

//    tile::tile GetChanTile(coords::global point, server::viewType::type view);
//    tile::tile GetSegTile(coords::global point, server::viewType::type view);
    int32_t GetSegId(coords::global point) const;

    void GetSegIds(coords::global point,
                   int radius,
                   server::viewType::type view,
                   std::set<int32_t>& ret) const;

    segment::data GetSegmentData(int32_t segId) const;
};

}} // namespace om::volume::
