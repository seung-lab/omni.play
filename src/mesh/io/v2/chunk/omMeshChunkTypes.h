#pragma once

struct OmMeshFilePart{
    uint64_t offsetIntoFile;
    uint64_t numElements;
    uint64_t totalBytes;
    uint64_t count;
};

struct OmMeshDataEntry {
    segId segID;
    bool wasMeshed;
    bool hasMeshData;
    OmMeshFilePart vertexIndex;
    OmMeshFilePart vertexData;
    OmMeshFilePart stripData;
    OmMeshFilePart trianData;
};

namespace om {
namespace meshio_ {

static OmMeshDataEntry MakeEmptyEntry(const segId segID)
{
    static const OmMeshFilePart empty = {0, 0, 0, 0};

    OmMeshDataEntry entry;
    entry.segID = segID;
    entry.wasMeshed = false;
    entry.hasMeshData = false;
    entry.vertexIndex = empty;
    entry.vertexData = empty;
    entry.stripData = empty;
    entry.trianData = empty;

    return entry;
}

} // namespace meshio_
} // namespace om

