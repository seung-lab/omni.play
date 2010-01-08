
#include "omMipMeshManager.h"

#include "omMipChunkMesher.h"
#include "omMipMesh.h"

#include "segment/omSegmentTypes.h"
#include "segment/omSegmentManager.h"

#include "volume/omMipChunkCoord.h"
#include "volume/omDrawOptions.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"

#include "common/omThreads.h"

#include <vtkImageData.h>
#include <QGLContext>



#define DEBUG 0




#pragma mark -
#pragma mark OmMipMeshManager
/////////////////////////////////
///////
///////		 OmMipMeshManager
///////

OmMipMeshManager::OmMipMeshManager() 
: MipMeshCache(VRAM_CACHE_GROUP, true) {
	
	//set cache properties
	SetFetchUpdateInterval(0.5f);
	//flushes fetch stack so it doesn't waste time fetching old requests
	SetFetchUpdateClearsFetchStack(true);
	
	//mesh data initially not built
	mMeshDataBuilt = false;
}


OmMipMeshManager::~OmMipMeshManager() {
	//delete context if valid
	//if(mFetchThreadContext) delete mFetchThreadContext;
}








#pragma mark 
#pragma mark Property Accessors
/////////////////////////////////
///////		 Property Accessors

/*
 *	MipMeshCache is in same directory as parent SegmentManager.
 */
const string& 
OmMipMeshManager::GetDirectoryPath() const {
	return mDirectoryPath;
}

void 
OmMipMeshManager::SetDirectoryPath(const string &dpath) {
	mDirectoryPath = dpath;
}








#pragma mark 
#pragma mark Mesh Accessors
/////////////////////////////////
///////		 Mesh Accessors

OmMipMesh*
OmMipMeshManager::AllocMesh(const OmMipMeshCoord &coord) {
	return new OmMipMesh(coord, this);
}

/*
 *	Get mesh from cache.
 */
shared_ptr<OmMipMesh>
OmMipMeshManager::GetMesh(const OmMipMeshCoord &coord) {
	return MipMeshCache::Get(coord);
}



void 
OmMipMeshManager::UncacheMesh(const OmMipMeshCoord &coord) {
	MipMeshCache::Remove(coord);
}




bool
OmMipMeshManager::IsMeshDataBuilt() {
	return mMeshDataBuilt;
}

void
OmMipMeshManager::SetMeshDataBuilt(bool status) {
	mMeshDataBuilt = status;
}







#pragma mark 
#pragma mark Build Meshes
/////////////////////////////////
///////		 Build Meshes


void 
OmMipMeshManager::BuildChunkMeshes(OmMipChunk& rMipChunk, const SegmentDataSet &rMeshVals ) {
	OmMipChunkMesher::BuildChunkMeshes(this, rMipChunk, rMeshVals);
}







#pragma mark 
#pragma mark Cache Handles
/////////////////////////////////
///////		 Cache Handles


/*
 *	Cache miss causes a fetch from disk for mesh that corresponds
 *	to the given MeshCoord.
 */
OmMipMesh* 
OmMipMeshManager::HandleCacheMiss(const OmMipMeshCoord &coord) {
	DOUT("OmMipMeshManager::HandleCacheMiss: " << coord);
	
//	DOUT("OmMipMeshManager::HandleCacheMiss: set context");
//	OmStateManager::MakeContextCurrent(mFetchThreadContext);
	
	//create mesh with this segment manager as cache
	DOUT("OmMipMeshManager::HandleCacheMiss: new mesh");
	OmMipMesh *mesh = AllocMesh(coord);
	
	//load data from disk
	DOUT("OmMipMeshManager::HandleCacheMiss: loading mesh from disk");
	mesh->Load();
	
	//upload vbo
	DOUT("OmMipMeshManager::HandleCacheMiss: creating vbo");
	//mesh->CreateVbo();
	
	//return mesh to cache
	DOUT("OmMipMeshManager::HandleCacheMiss: returning mesh");
	return mesh;
}


void 
OmMipMeshManager::HandleFetchUpdate() {
	DOUT("OmMipMeshManager::FetchUpdate: send redraw");
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}



bool
OmMipMeshManager::InitializeFetchThread() {
	DOUT("OmMipMeshManager::HandleCacheMiss: set context");

	//aquire and set shared context
	//mFetchThreadContext = OmStateManager::GetSharedView3dContext();
	//OmStateManager::MakeContextCurrent(mFetchThreadContext);
	
	return true;
}









#pragma mark 
#pragma mark Draw
/////////////////////////////////
///////		 Draw

void 
OmMipMeshManager::DrawMeshes(OmSegmentManager &rSegMgr,
							 const OmBitfield &drawOps,
							 const OmMipChunkCoord &mipCoord,
							 const SegmentDataSet &rRelvDataVals ) {
	
	//for all relevent data values in chunk
	SegmentDataSet::iterator itr;
	for( itr = rRelvDataVals.begin(); itr != rRelvDataVals.end(); itr++ ) {
		
		//get pointer to mesh
		shared_ptr<OmMipMesh> p_mesh = GetMesh( OmMipMeshCoord( mipCoord, *itr) );
		
		//if null pointer then skip to next mesh
		if(NULL == p_mesh.get()) continue;
		
		//determine which segment this data values belongs to
		OmId segment_id = rSegMgr.GetSegmentIdMappedToValue( *itr );
		
		//apply segment color
		OmSegment &r_segment = rSegMgr.GetSegment( segment_id );
		r_segment.ApplyColor(drawOps);
		
		//draw mesh
		glPushName(r_segment.GetId());
		glPushName(OMGL_NAME_MESH);
		
		p_mesh->Draw();
		
		glPopName();
		glPopName();
		
	}
}
