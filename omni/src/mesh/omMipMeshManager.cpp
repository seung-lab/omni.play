
#include "omMipMeshManager.h"

#include "omMipMesh.h"

#include "segment/omSegmentCache.h"

#include "volume/omMipChunkCoord.h"
#include "volume/omDrawOptions.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"

#include <vtkImageData.h>
#include <QGLContext>
#include "common/omDebug.h"

/////////////////////////////////
///////
///////          OmMipMeshManager
///////

OmMipMeshManager::OmMipMeshManager()
	: MipMeshCache(VRAM_CACHE_GROUP, true)
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
	MipMeshCache::Get(p_value, coord, false);
}

void OmMipMeshManager::UncacheMesh(const OmMipMeshCoord & coord)
{
	MipMeshCache::Remove(coord);
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

/////////////////////////////////
///////          Draw

void OmMipMeshManager::DrawMeshes(OmSegmentCache * rSegMgr,
				  const OmBitfield & drawOps,
				  const OmMipChunkCoord & mipCoord, 
				  const SegmentDataSet & rRelvDataVals)
{
	bool created = false;
	bool wasNotCreated = false;
	//debug("view3d", "in %s, about to draw %d chunks\n", __FUNCTION__, rRelvDataVals.size() );

	//for all relevent data values in chunk
	foreach( SEGMENT_DATA_TYPE val, rRelvDataVals ){

		//get pointer to mesh
		QExplicitlySharedDataPointer < OmMipMesh > p_mesh = QExplicitlySharedDataPointer < OmMipMesh > ();
		GetMesh(p_mesh, OmMipMeshCoord(mipCoord, val));

		//if null pointer then skip to next mesh
		if (NULL == p_mesh)
			continue;

		//determine which segment this data values belongs to
		OmSegment * r_segment = rSegMgr->GetSegmentFromValue(val);
		debug("segment", "drawing: %u:%u\n", r_segment->GetId(), val);

		//apply segment color
		r_segment->ApplyColor(drawOps);

		//draw mesh
		glPushName(r_segment->GetId());
		glPushName(OMGL_NAME_MESH);

		bool wasCreated;
		if (created) {
			wasCreated = p_mesh->Draw(false);
                        if (!wasCreated) {
                        	wasNotCreated = true;
                        }
		} else {
			wasCreated = p_mesh->Draw(true);
			if (wasCreated) {
				created = true;
			}
		}

		glPopName();
		glPopName();

	}

	if (wasNotCreated) {
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
	}
}
