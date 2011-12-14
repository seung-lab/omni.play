#pragma once


namespace om {
namespace datalayer {

struct ChunkId
{
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
}

struct Chunk
{
    int32_t VolumeId;
    ChunkId ChunkId;

};



}} // namespace om::datalayer::
