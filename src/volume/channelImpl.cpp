#include "volume/channelFolder.h"
#include "tiles/cache/tileCacheChannel.hpp"
#include "actions/omActions.h"
#include "chunks/chunk.h"
#include "chunks/chunkCache.hpp"
#include "chunks/chunkCache.hpp"
#include "common/common.h"
#include "common/omDebug.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/dataPath.h"
#include "datalayer/dataPaths.h"
#include "project/project.h"
#include "threads/omTaskManager.hpp"
#include "volume/io/volumeData.h"
#include "volume/channelImpl.h"
#include "volume/omFilter2d.h"
#include "zi/omThreads.h"

#include <float.h>

channelImpl::channelImpl()
    : chunkCache_(new chunkCache<channelImpl, chunk>(this))
    , volData_(new volumeData())
    , tileCache_(new tileCacheChannel())
{}

channelImpl::channelImpl(OmID id)
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
    const QString path = OmFileNames::GetVolDataFolderPath(this, 0);

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

chunk* channelImpl::GetChunk(const om::chunkCoord& coord){
    return chunkCache_->GetChunk(coord);
}

void channelImpl::UpdateFromVolResize(){
    chunkCache_->UpdateFromVolResize();
}
