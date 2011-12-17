#pragma once

#include "thrift/server_types.h"
#include "common/common.h"
#include "pipeline/mapData.hpp"
#include "segment/segmentTypes.hpp"
#include "datalayer/cache.h"
#include "chunks/chunk.hpp"

namespace om {
namespace volume {

class volume
{
private:
    int32_t id_;
    server::dataType::type channelType_; // Replace with omni type?
    server::dataType::type segmentationType_; // Replace with omni type?
    coords::volumeSystem coordSystem_;
    datalayer::cache cache_;

public:
    volume(const server::metadata& metadata);

    inline server::dataType::type ChannelType() const {
        return channelType_;
    }

    inline server::dataType::type SegmentationType() const {
        return segmentationType_;
    }

    inline const coords::volumeSystem& CoordSystem() const {
        return coordSystem_;
    }

    tiles::Tile GetChanTile(coords::global point, common::viewType view);
    tiles::Tile GetSegTile(coords::global point, common::viewType view);
    int32_t GetSegId(coords::global point);

    void GetSegIds(coords::global point,
                   int radius,
                   common::viewType view,
                   std::set<int32_t>& ret);

    segment::data GetSegmentData(int32_t segId);

private:
    chunks::ChunkVar getChunk(coords::global point,
                              chunks::ChunkType chunkType,
                              server::dataType::type type)
    {
        coords::data dc = point.toData(&coordSystem_, 0);

        chunks::ChunkVar chunk;
        if(!getChunk(dc.toChunk(), chunkType, type, chunk)) {
            throw argException("Unable to load chunk.");
        }

        return chunk;
    }

    bool getChunk(coords::chunk coord,
                  chunks::ChunkType chunkType,
                  server::dataType::type type,
                  chunks::ChunkVar& ret)
    {
        std::string key = chunks::Chunk<int8_t>::GetKey(id_, coord, chunkType);

        switch(type)
        {
        case server::dataType::INT8:
            return get<chunks::Chunk<int8_t> >(key, ret); break;
        case server::dataType::UINT8:
            return get<chunks::Chunk<uint8_t> >(key, ret); break;
        case server::dataType::INT32:
            return get<chunks::Chunk<int32_t> >(key, ret); break;
        case server::dataType::UINT32:
            return get<chunks::Chunk<uint32_t> >(key, ret); break;
        case server::dataType::FLOAT:
            return get<chunks::Chunk<float> >(key, ret); break;
        }

        return false;
    }

    template<typename T, typename U>
    bool get(std::string key, U& ret)
    {
        T* item = cache_.Get<T>(key);

        if(!item) {
            return false;
        }

        ret = *item;
        return true;
    }
};

}} // namespace om::volume::
