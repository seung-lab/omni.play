#ifndef OM_VOLUME_DATA_HPP
#define OM_VOLUME_DATA_HPP

#include "volume/io/omMemMappedVolume.h"

class OmMipVolume;

class OmVolumeData {
private:
    OmMemMappedVolume memMapped_;

public:
    OmVolumeData()
    {}

    void load(OmMipVolume* vol){
        memMapped_.load(vol);
    }

    void create(OmMipVolume* vol, const std::map<int, Vector3i>& levDims){
        memMapped_.create(vol, levDims);
    }

    int GetBytesPerVoxel() const{
        return memMapped_.GetBytesPerVoxel();
    }

    OmRawDataPtrs GetVolPtr(const int level){
        return memMapped_.GetVolPtr(level);
    }

    OmRawDataPtrs getChunkPtrRaw(const OmChunkCoord& coord){
        return memMapped_.getChunkPtrRaw(coord);
    }

    void downsample(OmMipVolume* vol){
        memMapped_.downsample(vol);
    }

    void SetDataType(OmMipVolume* vol){
        memMapped_.SetDataType(vol);
    }
};

#endif
