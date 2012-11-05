#pragma once

#include "chunks/omChunk.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"

class OmHdf5ChunkUtils {
public:
    static OmVolDataType DetermineOldVolType(OmMipVolume* vol)
    {
        if(!OmProject::HasOldHDF5()){
            throw IoException("no hdf5 to read from!");
        }
        OmHdf5* reader = OmProject::OldHDF5();

        const OmDataPath path = OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(),
                                                         0);

        OmDataWrapperPtr data =	reader->GetChunkDataType(path);

        return data->getVolDataType();
    }

    static OmDataWrapperPtr ReadChunkData(OmMipVolume* vol,
                                          OmChunk* chunk)
    {
        if(!OmProject::HasOldHDF5()){
            throw IoException("no hdf5 to read from!");
        }
        OmHdf5* reader = OmProject::OldHDF5();

        const OmDataPath path = OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(),
                                                         chunk->GetLevel());

        OmDataWrapperPtr data = reader->readChunk(path,
                                                  chunk->Mipping().GetExtent(),
                                                  om::common::NO_AFFINITY);

        return data;
    }

    template <typename VOL>
    static bool VolumeExistsInHDF5(VOL* vol, const int mipLevel)
    {
        if(!OmProject::HasOldHDF5()){
            throw IoException("no hdf5 to read from!");
        }
        OmHdf5* reader = OmProject::OldHDF5();

        const OmDataPath path = OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(),
                                                         mipLevel);

        return reader->dataset_exists(path);
    }
};

