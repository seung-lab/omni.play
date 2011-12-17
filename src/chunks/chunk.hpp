#pragma once

#include <boost/scoped_ptr.hpp>
#include <boost/variant.hpp>
#include <boost/format.hpp>

#include "common/common.h"
#include "datalayer/types.hpp"
#include "datalayer/cachedType.hpp"

namespace om {
namespace chunks {

enum ChunkType { CHANNEL, SEGMENTATION, AFFINITY_X, AFFINITY_Y, AFFINITY_Z };

template<typename T>
class Chunk : datalayer::cachedType<datalayer::Chunk<T> >
{
private:
    typedef datalayer::cachedType<datalayer::Chunk<T> > base_t;

public:
    Chunk() : base_t()
    { }

    Chunk(char* data, size_t size)
        : base_t(data, size)
    {

    }

    coords::chunk Coord() const
    {
        return coords::chunk(this->data_->Coord.Level,
                             this->data_->Coord.X,
                             this->data_->Coord.Y,
                             this->data_->Coord.Z);
    }

    const T& operator[](uint i) const
    {
        if(i >= 128*128*128) {
            throw argException("chunk index out of bounds.");
        }

        return this->data_->Data[i];
    }

    T& operator[](uint i) {
        if(i >= 128*128*128) {
            throw argException("chunk index out of bounds.");
        }

        return this->data_->Data[i];
    }

    T const * const ptr() const
    {
        return this->data_->Data;
    }

    static std::string GetKey(int volumeId, coords::chunk chunkCoord, ChunkType type)
    {
        return str(boost::format("%1%(Chunk):%2%:%3%-%4%-%5%")
                   % volumeId
                   % type
                   % chunkCoord.X() % chunkCoord.Y() % chunkCoord.Z());
    }
};

typedef boost::variant<Chunk<int8_t>,
                       Chunk<uint8_t>,
                       Chunk<int32_t>,
                       Chunk<uint32_t>,
                       Chunk<float> > ChunkVar;

}} // namespace om::chunks::
