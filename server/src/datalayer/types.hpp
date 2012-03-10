#pragma once

#include "segment/segmentTypes.hpp"

namespace om {
namespace datalayer {

struct ChunkId
{
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
}

template<typename T>
struct Chunk
{
    int32_t VolumeId;
    ChunkId ChunkId;
    T data[128*128*128];
};

struct SegmentationData
{
    uint8_t PageNum;
    segment::data Data[100000];
};

enum MeshDataType { MESH_ALLOC_TABLE = 1, MESH_DATA = 2 }

struct MeshHeader
{
    int32_t VolumeId;
    ChunkId ChunkId;
    MeshDataType MeshDataType;
};


}} // namespace om::datalayer::
