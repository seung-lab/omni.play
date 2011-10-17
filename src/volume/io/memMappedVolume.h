#pragma once

#include "volume/io/memMappedVolumeImpl.hpp"
#include "volume/volume.h"
#include "volume/volumeTypes.h"

namespace om {
namespace volume {

template<typename T>
class memMappedVolume {
public:
    memMappedVolume()
    {}

    void load(volume* vol)
    {
        volData_ = memMappedVolume<T>(vol);
        loadMemMapFiles();
        printf("loaded data\n");
    }

    void create(volume* vol, const std::map<int, Vector3i>& levDims)
    {
        volData_ = memMappedVolume<T>(vol);
        allocMemMapFiles(levDims);
    }

    int GetBytesPerVoxel() const;
    T* GetVolPtr(const int level);
    T* GetChunkPtr(const coords::chunkCoord& coord);

private:
    memMappedVolumeImpl<T> volData_;

    void loadMemMapFiles();
    void allocMemMapFiles(const std::map<int, Vector3i>& levDims);
};

} // namespace volume
} // namespace om
