#pragma once

#include "segment/segmentTypes.hpp"

namespace om {
namespace datalayer {

struct ChunkCoord
{
    uint8_t Level;
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
};

template<typename T>
struct Chunk
{
    int32_t VolumeId;
    ChunkCoord Coord;
    T Data[128*128*128];
};

struct SegmentationData
{
    int32_t VolumeId;
    uint8_t PageNum;
    segment::data Data[100000];
};

enum MeshDataType { MESH_ALLOC_TABLE = 1, MESH_DATA = 2 };

struct MeshHeader
{
    int32_t VolumeId;
    ChunkCoord Coord;
    MeshDataType Type;
};


}} // namespace om::datalayer::
