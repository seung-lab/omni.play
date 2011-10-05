#pragma once

#include "common/common.h"
#include "boost/shared_ptr.hpp"
#include "volume/volumeTypes.h"
#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmHdf5;
class OmChunk;
class OmVolume;
class OmVolumeData;

namespace YAML { template <class T> class mipVolume; }

namespace om {
namespace volume {

enum MipVolumeBuildState { MIPVOL_UNBUILT = 0,
                           MIPVOL_BUILT,
                           MIPVOL_BUILDING };

class volume {
public:
    volume();
    virtual ~volume(){}

    virtual std::string GetDirectoryPath() const = 0;
    virtual std::string GetName() = 0;
    virtual bool LoadVolData() = 0;
    virtual OmVolumeData* VolData() = 0;
    virtual common::objectType getVolumeType() const = 0;
    virtual common::id getID() const = 0;

    inline bool IsBuilt(){
        return MIPVOL_BUILT == mBuildState;
    }

    boost::shared_ptr<std::deque<coords::chunkCoord> > GetMipChunkCoords() const;
    boost::shared_ptr<std::deque<coords::chunkCoord> > GetMipChunkCoords(const int mipLevel) const;

    //mip data accessors
    bool ContainsVoxel(const coords::globalCoord &vox);

    volDataType getVolDataType(){
        return mVolDataType;
    }

    std::string getVolDataTypeAsStr(){
        return volumeTypeHelpers::GetTypeAsString(mVolDataType);
    }

    virtual void SetVolDataType(const OmVolDataType) = 0;

    virtual int GetBytesPerVoxel() const = 0;
    virtual int GetBytesPerSlice() const = 0;

    inline coords::volumeSystem& CoordinateSystem() {
        return coords_;
    }

    inline const coords::volumeSystem& CoordinateSystem() const {
        return coords_;
    }

protected:
    coords::volumeSystem coords_;
    volDataType mVolDataType;

    int mBuildState;
    void SetBuildState(const MipVolumeBuildState s){
        mBuildState = s;
    }

    void addChunkCoordsForLevel(const int mipLevel,
                                std::deque<coords::chunkCoord>* coords) const;

private:
//     template <class T> friend class OmVolumeBuilderBase;
//     template <class T> friend class OmVolumeBuilderHdf5;
//     template <class T> friend class OmVolumeBuilderImages;
//     template <class T> friend class OmMipVolumeArchive;
//     template <class T> friend class YAML::mipVolume;
// 
//     friend class OmChunk;
//     friend class OmMipVolumeArchiveOld;
//     friend class OmVolumeData;
//     friend class OmMemMappedVolume;
//     friend class OmUpgradeTo14;
//     friend class OmWatershedImporter;
};

} // namespace volume
} // namespace om
