#pragma once
#include "precomp.h"

#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "project/omProject.h"
#include "volume/omMipVolume.h"

class OmHdf5ChunkUtils {
 public:
  static om::common::DataType DetermineOldVolType(OmMipVolume* vol) {
    if (!OmProject::HasOldHDF5()) {
      throw om::IoException("no hdf5 to read from!");
    }
    OmHdf5* reader = OmProject::OldHDF5();

    const OmDataPath path =
        OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(), 0);

    OmDataWrapperPtr data = reader->GetChunkDataType(path);

    return data->getVolDataType();
  }

  static OmDataWrapperPtr ReadChunkData(OmMipVolume* vol,
                                        const om::coords::Chunk& chunk) {
    if (!OmProject::HasOldHDF5()) {
      throw om::IoException("no hdf5 to read from!");
    }
    OmHdf5* reader = OmProject::OldHDF5();

    const OmDataPath path =
        OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(), chunk.mipLevel());

    OmDataWrapperPtr data =
        reader->readChunk(path, chunk.BoundingBox(vol->Coords()),
                          om::common::AffinityGraph::NO_AFFINITY);

    return data;
  }

  template <typename VOL>
  static bool VolumeExistsInHDF5(VOL* vol, const int mipLevel) {
    if (!OmProject::HasOldHDF5()) {
      throw om::IoException("no hdf5 to read from!");
    }
    OmHdf5* reader = OmProject::OldHDF5();

    const OmDataPath path =
        OmDataPaths::Hdf5VolData(vol->GetDirectoryPath(), mipLevel);

    return reader->dataset_exists(path);
  }
};
