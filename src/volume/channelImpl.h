#pragma once

#include "common/std.h"
#include "common/manageableObject.hpp"
#include "volume/mipVolume.h"

class volumeData;

namespace om { namespace channel { class folder; } }

namespace om {
namespace volume {

class channelImpl : public volume, public common::manageableObject {

public:
    channelImpl();
    channelImpl(common::id id);
    virtual ~channelImpl();

    virtual std::string GetDefaultHDF5DatasetName() = 0;

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

    common::objectType getVolumeType() const {
        return common::CHANNEL;
    }

    common::id getID() const {
        return GetID();
    }

    virtual int GetBytesPerVoxel() const;
    virtual int GetBytesPerSlice() const;

    void CloseDownThreads();

    void SetVolDataType(const dataType);

    chunk* GetChunk(const coords::chunkCoord& coord);

    inline om::channel::folder* Folder() const {
        return folder_.get();
    }

protected:
    //protected copy constructor and assignment operator to prevent copy
    channelImpl(const channelImpl&);
    channelImpl& operator= (const channelImpl&);

    boost::scoped_ptr<om::channel::folder> folder_;
    boost::scoped_ptr<volumeData> volData_;   
};

} // namespace volume
} // namespace om