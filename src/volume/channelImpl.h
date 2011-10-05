#pragma once

/*
 * channel is the MIP data structure for a raw data volume
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "common/omStd.h"
#include "system/omManageableObject.h"
#include "volume/omFilter2dManager.h"
#include "volume/mipVolume.h"

class tileCacheChannel;
class volumeData;
template <typename,typename> class chunkCache;

namespace om { namespace channel { class folder; } }

class channelImpl : public mipVolume, public OmManageableObject {

public:
    channelImpl();
    channelImpl(OmID id);
    virtual ~channelImpl();

    virtual QString GetDefaultHDF5DatasetName() = 0;

    volumeData* VolData() {
        return volData_.get();
    }

    std::string GetName();
    std::string GetNameHyphen();
    std::string GetDirectoryPath() const;
    void LoadPath();

    bool LoadVolData();
    bool LoadVolDataIfFoldersExist();
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

    chunk* GetChunk(const om::chunkCoord& coord);

    inline std::vector<OmFilter2d*> GetFilters() const {
        return filterManager_.GetFilters();
    }

    chunkCache<channelImpl, chunk>* ChunkCache(){
        return chunkCache_.get();
    }

    inline tileCacheChannel* TileCache(){
        return tileCache_.get();
    }

    inline om::channel::folder* Folder() const {
        return folder_.get();
    }

protected:
    //protected copy constructor and assignment operator to prevent copy
    channelImpl(const channelImpl&);
    channelImpl& operator= (const channelImpl&);

    boost::scoped_ptr<om::channel::folder> folder_;
    boost::scoped_ptr<chunkCache<channelImpl, chunk> > chunkCache_;
    boost::scoped_ptr<volumeData> volData_;
    boost::scoped_ptr<tileCacheChannel> tileCache_;

    OmFilter2dManager filterManager_;

private:
    friend class channelImplChunkBuildTask;
    friend class dataArchiveProjectImpl;
};

