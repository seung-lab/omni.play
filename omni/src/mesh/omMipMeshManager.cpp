#include "common/omDebug.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "segment/omSegmentCache.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "system/omThreadedCache.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omDrawOptions.h"
#include "volume/omMipChunkCoord.h"

#include <vtkImageData.h>
#include <QGLContext>

/////////////////////////////////
///////          OmMipMeshManager

OmMipMeshManager::OmMipMeshManager()
	: OmThreadedCache<OmMipMeshCoord, OmMipMesh>(VRAM_CACHE_GROUP, true)
{
	// set cache properties
	SetFetchUpdateInterval(0.5f);

	// flushes fetch stack so it doesn't waste time fetching old requests
	SetFetchUpdateClearsFetchStack(true);

        SetCacheName("OmMipMeshManager");
}

OmMipMeshManager::~OmMipMeshManager()
{
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

OmMipMesh *OmMipMeshManager::AllocMesh(const OmMipMeshCoord & coord)
{
	return new OmMipMesh(coord, this);
}

/*
 *	Get mesh from cache.
 */
void OmMipMeshManager::GetMesh(QExplicitlySharedDataPointer < OmMipMesh > &p_value, const OmMipMeshCoord & coord)
{
	OmThreadedCache<OmMipMeshCoord, OmMipMesh>::Get(p_value, coord, false);
}

void OmMipMeshManager::UncacheMesh(const OmMipMeshCoord & coord)
{
	OmThreadedCache<OmMipMeshCoord, OmMipMesh>::Remove(coord);
}

/////////////////////////////////
///////          Cache Handles

/*
 *	Cache miss causes a fetch from disk for mesh that corresponds
 *	to the given MeshCoord.
 */
OmMipMesh * OmMipMeshManager::HandleCacheMiss(const OmMipMeshCoord & coord)
{
	//create mesh with this segment manager as cache
	OmMipMesh *mesh = AllocMesh(coord);

	//load data from disk
	try {
		mesh->Load();
	} catch (...) {
	}

	//return mesh to cache
	return mesh;
}

void OmMipMeshManager::HandleFetchUpdate()
{
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW_CACHE));
}

bool OmMipMeshManager::InitializeFetchThread()
{
	return true;
}
