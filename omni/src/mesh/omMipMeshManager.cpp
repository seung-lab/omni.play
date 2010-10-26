#include "common/omDebug.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "system/cache/omThreadedCache.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omSegmentation.h"
#include "system/cache/omMeshCache.h"

OmMipMeshManager::OmMipMeshManager(OmSegmentation* segmentation)
	: segmentation_(segmentation)
	, mDataCache(new OmMeshCache(this))
{}

OmMipMeshManager::~OmMipMeshManager(){
	delete mDataCache;
}

OmMipMeshPtr OmMipMeshManager::AllocMesh(const OmMipMeshCoord& coord){
	return boost::make_shared<OmMipMesh>(coord, this, mDataCache);
}

void OmMipMeshManager::GetMesh(OmMipMeshPtr& p_value,
							   const OmMipMeshCoord & coord)
{
	mDataCache->Get(p_value, coord, om::NON_BLOCKING);
}

void OmMipMeshManager::UncacheMesh(const OmMipMeshCoord & coord){
	mDataCache->Remove(coord);
}

void OmMipMeshManager::CloseDownThreads(){
	mDataCache->closeDownThreads();
}

std::string OmMipMeshManager::GetDirectoryPath() const {
	return segmentation_->GetDirectoryPath();
}
