#include "actions/omActions.h"
#include "chunks/omChunk.h"
#include "chunks/omChunkCache.hpp"
#include "chunks/omChunkCache.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "utility/omThreadPool.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omChannelImpl.h"
#include "volume/omFilter2d.h"
#include "zi/omThreads.h"

#include <float.h>

OmChannelImpl::OmChannelImpl()
    : chunkCache_(new OmChunkCache<OmChannelImpl, OmChunk>(this))
    , volData_(new OmVolumeData())
{}

OmChannelImpl::OmChannelImpl(OmID id)
    : OmManageableObject(id)
    , chunkCache_(new OmChunkCache<OmChannelImpl, OmChunk>(this))
    , volData_(new OmVolumeData())
{
    filterManager_.AddFilter();
}

OmChannelImpl::~OmChannelImpl()
{}

std::string OmChannelImpl::GetName(){
    return "channel" + om::string::num(GetID());
}

std::string OmChannelImpl::GetNameHyphen(){
    return "channel-" + om::string::num(GetID());
}

std::string OmChannelImpl::GetDirectoryPath() {
    return OmDataPaths::getDirectoryPath(this);
}

void OmChannelImpl::CloseDownThreads()
{}

void OmChannelImpl::loadVolData()
{
    if(IsBuilt())
    {
        UpdateFromVolResize();
        volData_->load(this);
    }
}

void OmChannelImpl::loadVolDataIfFoldersExist()
{
    //assume level 0 data always present
    const QString path = OmFileNames::GetVolDataFolderPath(this, 0);

    if(QDir(path).exists()){
        loadVolData();
    }
}

int OmChannelImpl::GetBytesPerVoxel() const{
    return volData_->GetBytesPerVoxel();
}

int OmChannelImpl::GetBytesPerSlice() const {
    return GetBytesPerVoxel()*128*128;
}

void OmChannelImpl::SetVolDataType(const OmVolDataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

void OmChannelImpl::GetChunk(OmChunkPtr& ptr, const OmChunkCoord& coord){
    chunkCache_->GetChunk(ptr, coord);
}

void OmChannelImpl::UpdateFromVolResize(){
    chunkCache_->UpdateFromVolResize();
}
