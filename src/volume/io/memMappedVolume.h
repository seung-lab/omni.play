#pragma once

#include "volume/io/memMappedVolumeImpl.hpp"
#include "volume/volume.h"
#include "volume/volumeTypes.h"

namespace om {
namespace volume {

class memMappedVolume {
public:
    memMappedVolume()
    {}

    template <typename VOL>
    void load(VOL* vol)
    {
        SetDataType(vol);
        loadMemMapFiles();
        printf("loaded data\n");
    }

    template <typename VOL>
    void create(VOL* vol, const std::map<int, Vector3i>& levDims)
    {
        SetDataType(vol);
        allocMemMapFiles(levDims);
    }

    int GetBytesPerVoxel() const;
    rawDataPtrs GetVolPtr(const int level);
    rawDataPtrs getChunkPtrRaw(const coords::chunkCoord& coord);

    void downsample(volume* vol);

    template <typename VOL>
    void SetDataType(VOL* vol)
    {
        printf("setting up volume data...\n");

        volData_ = makeVolData(vol);
    }

private:
    volDataSrcs volData_;

    void loadMemMapFiles();
    void allocMemMapFiles(const std::map<int, Vector3i>& levDims);
    rawDataPtrs GetVolPtrType();

    volDataSrcs makeVolData(volume* vol)
    {
        switch(vol->mVolDataType.index()){
        case dataType::INT8:
            return memMappedVolumeImpl<int8_t>(vol);
        case dataType::UINT8:
            return memMappedVolumeImpl<uint8_t>(vol);
        case dataType::INT32:
            return memMappedVolumeImpl<int32_t>(vol);
        case dataType::UINT32:
            return memMappedVolumeImpl<uint32_t>(vol);
        case dataType::FLOAT:
            return memMappedVolumeImpl<float>(vol);
        case dataType::UNKNOWN:
            throw common::ioException("unknown data type--probably old file?");
        }

        throw common::argException("type not know");
    }
};

} // namespace volume
} // namespace om
