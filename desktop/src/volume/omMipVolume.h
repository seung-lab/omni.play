#pragma once

/*
 *  OmMipVolume is composed of sub-volumes cubes call OmChunks.
 *
 *  Brett Warne - bwarne@mit.edu - 7/19/09
 */

#include "common/common.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolCoordsMipped.hpp"

class OmDataPath;
class OmHdf5;
class OmChunk;
class OmVolume;
class OmVolumeData;

namespace YAML { template <class T> class mipVolume; }

namespace om{ namespace rebuilder { class segmentation; } }

enum MipVolumeBuildState { MIPVOL_UNBUILT = 0,
                           MIPVOL_BUILT,
                           MIPVOL_BUILDING };

class OmMipVolume {
public:
    OmMipVolume();
    virtual ~OmMipVolume(){}

    virtual std::string GetDirectoryPath() const = 0;
    virtual std::string GetName() = 0;
    virtual bool LoadVolData() = 0;
    virtual OmVolumeData* VolData() = 0;
    virtual ObjectType getVolumeType() const = 0;
    virtual om::common::ID getID() const = 0;

    inline bool IsBuilt(){
        return MIPVOL_BUILT == mBuildState;
    }

    om::shared_ptr<std::deque<om::coords::Chunk> > MipChunkCoords() const;
    om::shared_ptr<std::deque<om::coords::Chunk> > MipChunkCoords(const int mipLevel) const;

    //mip data accessors
    bool ContainsVoxel(const om::coords::Global &vox);

    OmVolDataType getVolDataType(){
        return mVolDataType;
    }

    std::string getVolDataTypeAsStr(){
        return OmVolumeTypeHelpers::GetTypeAsString(mVolDataType);
    }

    virtual void SetVolDataType(const OmVolDataType) = 0;

    virtual int GetBytesPerVoxel() const = 0;
    virtual int GetBytesPerSlice() const = 0;

    inline OmMipVolCoords& Coords() {
        return coords_;
    }

    inline const OmMipVolCoords& Coords() const {
        return coords_;
    }

protected:
    //OmMipVolCoords coords_;
    OmVolDataType mVolDataType;

    OmMipVolCoords coords_;

    int mBuildState;
    void SetBuildState(const MipVolumeBuildState s){
        mBuildState = s;
    }

    void addChunkCoordsForLevel(const int mipLevel,
                                std::deque<om::coords::Chunk>* coords) const;

private:
    template <class T> friend class OmVolumeBuilderBase;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmMipVolumeArchive;
    template <class T> friend class YAML::mipVolume;

    friend class OmChunk;
    friend class OmMipVolumeArchiveOld;
    friend class OmVolumeData;
    friend class OmMemMappedVolume;
    friend class OmUpgradeTo14;
    friend class OmWatershedImporter;
    friend class om::rebuilder::segmentation;
};

