#ifndef OM_MIP_VOLUME_H
#define OM_MIP_VOLUME_H

/*
 *  OmMipVolume is composed of sub-volumes cubes call OmChunks.
 *
 *  Brett Warne - bwarne@mit.edu - 7/19/09
 */

#include "common/omCommon.h"
#include "chunks/omChunkCoord.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolCoordsMipped.hpp"

class OmDataPath;
class OmHdf5;
class OmChunk;
class OmVolume;
class OmVolumeData;

enum MipVolumeBuildState { MIPVOL_UNBUILT = 0,
                           MIPVOL_BUILT,
                           MIPVOL_BUILDING };

class OmMipVolume {
public:
    OmMipVolume();
    virtual ~OmMipVolume(){}

    virtual std::string GetDirectoryPath() = 0;
    virtual std::string GetName() = 0;
    virtual void loadVolData() = 0;
    virtual OmVolumeData* VolData() = 0;
    virtual ObjectType getVolumeType() = 0;
    virtual OmID getID() = 0;

    inline bool IsBuilt(){
        return MIPVOL_BUILT == mBuildState;
    }

    boost::shared_ptr<std::deque<OmChunkCoord> > GetMipChunkCoords() const;
    boost::shared_ptr<std::deque<OmChunkCoord> > GetMipChunkCoords(const int mipLevel) const;

    //mip data accessors
    bool ContainsVoxel(const DataCoord &vox);

    OmVolDataType getVolDataType(){
        return mVolDataType;
    }

    std::string getVolDataTypeAsStr(){
        return OmVolumeTypeHelpers::GetTypeAsString(mVolDataType);
    }

    virtual void SetVolDataType(const OmVolDataType) = 0;

    virtual int GetBytesPerVoxel() const = 0;

    inline OmMipVolCoords& Coords() {
        return coords_;
    }

    inline const OmMipVolCoords& Coords() const {
        return coords_;
    }

protected:
    OmMipVolCoords coords_;
    OmVolDataType mVolDataType;

    int mBuildState;
    void SetBuildState(const MipVolumeBuildState s){
        mBuildState = s;
    }

private:
    template <class T> friend class OmVolumeBuilderBase;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;

    friend class OmChunk;
    friend class OmMipVolumeArchive;
    friend class OmMipVolumeArchiveOld;
    friend class OmVolumeData;
    friend class OmMemMappedVolume;
    friend class OmUpgradeTo14;
    friend class OmWatershedImporter;
};

#endif

