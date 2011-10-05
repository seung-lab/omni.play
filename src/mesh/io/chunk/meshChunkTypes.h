#pragma once

struct meshFilePart{
    uint64_t offsetIntoFile;
    uint64_t numElements;
    uint64_t totalBytes;
    uint64_t count;
};

struct meshDataEntry {
    segId segID;
    bool wasMeshed;
    bool hasMeshData;
    meshFilePart vertexIndex;
    meshFilePart vertexData;
    meshFilePart stripData;
    meshFilePart trianData;
};

namespace om {
namespace meshio_ {

static meshDataEntry MakeEmptyEntry(const segId segID)
{
    static const meshFilePart empty = {0, 0, 0, 0};

    meshDataEntry entry;
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

