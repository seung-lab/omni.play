#pragma once

#include "volume/io/omMemMappedVolumeImpl.hpp"
#include "volume/mipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/hdf5/omHdf5ChunkUtils.hpp"

class OmMemMappedVolume {
public:
    OmMemMappedVolume()
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
    OmRawDataPtrs GetVolPtr(const int level);
    OmRawDataPtrs getChunkPtrRaw(const om::chunkCoord& coord);

    void downsample(OmMipVolume* vol);

    template <typename VOL>
    void SetDataType(VOL* vol)
    {
        printf("setting up volume data...\n");

        if(OmVolDataType::UNKNOWN == vol->mVolDataType.index())
        {
            printf("unknown data type--old file? attempting to infer type...\n");

            if(OmProject::HasOldHDF5()){
                vol->mVolDataType = OmHdf5ChunkUtils::DetermineOldVolType(vol);

            } else {
                throw OmIoException("can not resolve volume type");
            }
        }

        volData_ = makeVolData(vol);
    }

private:
    OmVolDataSrcs volData_;

    void loadMemMapFiles();
    void allocMemMapFiles(const std::map<int, Vector3i>& levDims);
    OmRawDataPtrs GetVolPtrType();

    OmVolDataSrcs makeVolData(OmMipVolume* vol)
    {
        switch(vol->mVolDataType.index()){
        case OmVolDataType::INT8:
            return OmMemMappedVolumeImpl<int8_t>(vol);
        case OmVolDataType::UINT8:
            return OmMemMappedVolumeImpl<uint8_t>(vol);
        case OmVolDataType::INT32:
            return OmMemMappedVolumeImpl<int32_t>(vol);
        case OmVolDataType::UINT32:
            return OmMemMappedVolumeImpl<uint32_t>(vol);
        case OmVolDataType::FLOAT:
            return OmMemMappedVolumeImpl<float>(vol);
        case OmVolDataType::UNKNOWN:
            throw OmIoException("unknown data type--probably old file?");
        }

        throw OmArgException("type not know");
    }
};

