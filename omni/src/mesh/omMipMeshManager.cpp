#include "common/omDebug.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "segment/omSegmentCache.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "system/cache/omThreadedCache.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omDrawOptions.h"
#include "volume/omMipChunkCoord.h"
#include "system/cache/omMeshCache.h"

#include <QGLContext>

/////////////////////////////////
///////          OmMipMeshManager

OmMipMeshManager::OmMipMeshManager()
	: mDataCache(new OmMeshCache(this))
{
}

OmMipMeshManager::~OmMipMeshManager()
{
	delete mDataCache;
}

/////////////////////////////////
///////          Property Accessors

/*
 *	MipMeshCache is in same directory as parent SegmentManager.
 */
const QString & OmMipMeshManager::GetDirectoryPath() const
{
	return mDirectoryPath;
}

void OmMipMeshManager::SetDirectoryPath(const QString & dpath)
{
	mDirectoryPath = dpath;
}

/////////////////////////////////
///////          Mesh Accessors

OmMipMeshPtr OmMipMeshManager::AllocMesh(const OmMipMeshCoord & coord)
{
	return boost::make_shared<OmMipMesh>(coord, this, mDataCache);
}

/*
 *	Get mesh from cache.
 */
void OmMipMeshManager::GetMesh(OmMipMeshPtr& p_value, const OmMipMeshCoord & coord)
{
	mDataCache->Get(p_value, coord, false);
}

void OmMipMeshManager::UncacheMesh(const OmMipMeshCoord & coord)
{
	mDataCache->Remove(coord);
}

void OmMipMeshManager::CloseDownThreads()
{
	mDataCache->closeDownThreads();
}
