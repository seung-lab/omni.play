#pragma once

#include "volume/io/memMappedVolume.h"

class volumeData {
private:
    memMappedVolume memMapped_;

public:
    volumeData()
    {}

    template <typename VOL>
    void load(VOL* vol){
        memMapped_.load(vol);
    }

    template <typename VOL>
    void create(VOL* vol, const std::map<int, Vector3i>& levDims){
        memMapped_.create(vol, levDims);
    }

    int GetBytesPerVoxel() const{
        return memMapped_.GetBytesPerVoxel();
    }

    OmRawDataPtrs GetVolPtr(const int level){
        return memMapped_.GetVolPtr(level);
    }

    OmRawDataPtrs getChunkPtrRaw(const coords::chunkCoord& coord){
        return memMapped_.getChunkPtrRaw(coord);
    }

    template <typename VOL>
    void downsample(VOL* vol){
        memMapped_.downsample(vol);
    }

    template <typename VOL>
    void SetDataType(VOL* vol){
        memMapped_.SetDataType(vol);
    }
};

