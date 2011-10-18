#include "volume/channelFolder.h"
#include "chunks/chunk.h"
#include "common/common.h"
#include "common/debug.h"
#include "datalayer/fs/fileNames.hpp"
#include "datalayer/dataPaths.h"
#include "project/project.h"
#include "threads/taskManager.hpp"
#include "volume/io/volumeData.h"
#include "volume/channelImpl.h"
#include "zi/threads.h"

#include <float.h>

namespace om {
namespace volume {

channelImpl::channelImpl()
    : volData_(new data())
{}

channelImpl::channelImpl(common::id id)
    : manageableObject(id)
    , volData_(new data())
{
    LoadPath();
}

channelImpl::~channelImpl()
{}

void channelImpl::LoadPath(){
    folder_.reset(new om::channel::folder(this));
}

std::string channelImpl::GetName(){
    return "channel" + om::string::num(GetID());
}

std::string channelImpl::GetNameHyphen(){
    return "channel-" + om::string::num(GetID());
}

std::string channelImpl::GetDirectoryPath() const {
    return folder_->RelativeVolPath();
}

void channelImpl::CloseDownThreads()
{}

bool channelImpl::LoadVolData()
{
    if(IsBuilt())
    {
        volData_->load(this);
        return true;
    }

    return false;
}

bool channelImpl::LoadVolDataIfFoldersExist()
{
    //assume level 0 data always present
    const std::string path = datalayer::fileNames::GetVolDataFolderPath(this, 0);

    if(file::exists(path))
    {
        return LoadVolData();
    }

    return false;
}

int channelImpl::GetBytesPerVoxel() const{
    return volData_->GetBytesPerVoxel();
}

int channelImpl::GetBytesPerSlice() const {
    return GetBytesPerVoxel()*128*128;
}

void channelImpl::SetVolDataType(const dataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

chunks::chunk* channelImpl::GetChunk(const coords::chunkCoord& coord) {
    return new chunks::chunk(this, coord);
}

} // namespace volume
} // namespace om
