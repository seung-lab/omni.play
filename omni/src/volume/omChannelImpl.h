#ifndef OM_CHANNEL_IMPL_H
#define OM_CHANNEL_IMPL_H

/*
 * OmChannel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "chunks/omChunkTypes.hpp"
#include "common/omStd.h"
#include "system/omManageableObject.h"
#include "volume/omFilter2dManager.h"
#include "volume/omMipVolume.h"

class OmVolumeData;
template <typename,typename> class OmChunkCache;

class OmChannelImpl : public OmMipVolume, public OmManageableObject {

public:
    OmChannelImpl();
    OmChannelImpl(OmID id);
    virtual ~OmChannelImpl();

    virtual QString GetDefaultHDF5DatasetName() = 0;

    OmVolumeData* VolData() {
        return volData_.get();
    }

    std::string GetName();
    std::string GetNameHyphen();
    std::string GetDirectoryPath();

    void loadVolData();
    void loadVolDataIfFoldersExist();
    void UpdateFromVolResize();

    ObjectType getVolumeType() const {
        return CHANNEL;
    }

    OmID getID() const {
        return GetID();
    }

    virtual int GetBytesPerVoxel() const;
    virtual int GetBytesPerSlice() const;

    void CloseDownThreads();

    OmFilter2dManager& FilterManager(){
        return filterManager_;
    }

    void SetVolDataType(const OmVolDataType);

    void GetChunk(OmChunkPtr& ptr, const OmChunkCoord& coord);

    inline std::vector<OmFilter2d*> GetFilters() const {
        return filterManager_.GetFilters();
    }

    OmChunkCache<OmChannelImpl, OmChunk>* ChunkCache(){
        return chunkCache_.get();
    }

protected:
    //protected copy constructor and assignment operator to prevent copy
    OmChannelImpl(const OmChannelImpl&);
    OmChannelImpl& operator= (const OmChannelImpl&);

    boost::scoped_ptr<OmChunkCache<OmChannelImpl, OmChunk> > chunkCache_;
    boost::scoped_ptr<OmVolumeData> volData_;

    OmFilter2dManager filterManager_;

private:
    friend class OmChannelImplChunkBuildTask;
    friend class OmDataArchiveProject;
};

#endif
