#pragma once

#include "common/common.h"
#include "boost/shared_ptr.hpp"
#include "volume/volumeTypes.h"
#include "datalayer/dataWrapper.h"

class dataPath;
class chunk;
class volumeData;

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
    virtual volumeData* VolData() = 0;
    virtual common::objectType getVolumeType() const = 0;
    virtual common::id getID() const = 0;

    inline bool IsBuilt(){
        return MIPVOL_BUILT == mBuildState;
    }

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
//     template <class T> friend class mipVolumeArchive;
//     template <class T> friend class YAML::mipVolume;
// 
//     friend class chunk;
//     friend class mipVolumeArchiveOld;
//     friend class volumeData;
//     friend class memMappedVolume;
//     friend class OmUpgradeTo14;
//     friend class OmWatershedImporter;
};

} // namespace volume
} // namespace om
