#include "volume/channelFolder.h"
#include "chunks/chunk.h"
#include "common/common.h"
#include "common/debug.h"
#include "datalayer/fs/fileNames.hpp"
#include "datalayer/dataPath.h"
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
    : chunkCache_(new chunkCache<channelImpl, chunk>(this))
    , volData_(new volumeData())
    , tileCache_(new tileCacheChannel())
{}

channelImpl::channelImpl(common::id id)
    : OmManageableObject(id)
    , chunkCache_(new chunkCache<channelImpl, chunk>(this))
    , volData_(new volumeData())
    , tileCache_(new tileCacheChannel())
{
    LoadPath();

    filterManager_.AddFilter();
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
    return folder_->RelativeVolPath().toStdString();
}

void channelImpl::CloseDownThreads()
{}

bool channelImpl::LoadVolData()
{
    if(IsBuilt())
    {
        UpdateFromVolResize();
        volData_->load(this);
        tileCache_->Load(this);
        return true;
    }

    return false;
}

bool channelImpl::LoadVolDataIfFoldersExist()
{
    //assume level 0 data always present
    const std::string path = fileNames::GetVolDataFolderPath(this, 0);

    if(QDir(path).exists())
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

void channelImpl::SetVolDataType(const OmVolDataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

chunk* channelImpl::GetChunk(const coords::chunkCoord& coord){
    return chunkCache_->GetChunk(coord);
}

void channelImpl::UpdateFromVolResize(){
    chunkCache_->UpdateFromVolResize();
}

} // namespace volume
} // namespace om