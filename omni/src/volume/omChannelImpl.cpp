#include "datalayer/fs/omFileNames.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "project/omProject.h"
#include "actions/omActions.h"
#include "chunks/omChunkCache.hpp"
#include "utility/omThreadPool.hpp"
#include "volume/omChannelImpl.h"
#include "volume/omFilter2d.h"
#include "chunks/omChunk.h"
#include "volume/io/omVolumeData.h"
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
	return "channel" + om::NumToStr(GetID());
}

std::string OmChannelImpl::GetDirectoryPath() {
	return OmDataPaths::getDirectoryPath(this);
}

void OmChannelImpl::CloseDownThreads()
{}

void OmChannelImpl::loadVolData()
{
	if(IsBuilt()){
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

void OmChannelImpl::SetVolDataType(const OmVolDataType type)
{
	mVolDataType = type;
	volData_->SetDataType(this);
}

void OmChannelImpl::GetChunk(OmChunkPtr& ptr, const OmChunkCoord& coord){
	chunkCache_->Get(ptr, coord);
}
