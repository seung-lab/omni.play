#pragma once

struct OmMeshFilePart{
    uint64_t offsetIntoFile;
    uint64_t numElements;
    uint64_t totalBytes;
    uint64_t count;
};

inline std::ostream& operator<<(std::ostream& out, const OmMeshFilePart& mfp)
{
	out << "\n\toffsetIntoFile:" << mfp.offsetIntoFile << std::endl;
    out << "\tnumElements:" << mfp.numElements << std::endl;
    out << "\ttotalBytes:" << mfp.totalBytes << std::endl;
    out << "\tcount:" << mfp.count << std::endl;
    return out;
}

struct OmMeshDataEntry {
    OmSegID segID;
    bool wasMeshed;
    bool hasMeshData;
    OmMeshFilePart vertexIndex;
    OmMeshFilePart vertexData;
    OmMeshFilePart stripData;
    OmMeshFilePart trianData;
};

inline std::ostream& operator<<(std::ostream& out, const OmMeshDataEntry& mde)
{
	out << "segID: " << mde.segID << std::endl;
    out << "wasMeshed: " << mde.wasMeshed << std::endl;
    out << "hasMeshData: " << mde.hasMeshData << std::endl;
    out << "vertexIndex: " << mde.vertexIndex << std::endl;
    out << "vertexData: " << mde.vertexData << std::endl;
    out << "stripData: " << mde.stripData << std::endl;
    out << "trianData: " << mde.trianData << std::endl;
    return out;
}

namespace om {
namespace meshio_ {

static OmMeshDataEntry MakeEmptyEntry(const OmSegID segID)
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

