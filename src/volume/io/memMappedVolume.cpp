#include "volume/io/memMappedVolume.h"
#include "chunks/chunk.h"

namespace om {
namespace volume {

void memMappedVolume::loadMemMapFiles()
{
    volData_.Load();
}

void memMappedVolume::allocMemMapFiles(const std::map<int, Vector3i>& levDims){
    volData_.Create(levDims);
}

int memMappedVolume::GetBytesPerVoxel() const {
    volData_.GetBytesPerVoxel();
}

template <typename T>
T* memMappedVolume::GetVolPtr(const int level) {
    volData_.GetPtr(level);
}

template <typename T>
T* memMappedVolume::GetChunkPtr(const coords::chunkCoord& coord){
    volData_.GetChunkPtr(coord);
}

}
}
