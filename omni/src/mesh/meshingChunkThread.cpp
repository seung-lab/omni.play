#include "meshingChunkThread.h"
#include "meshingChunkThreadManager.h"

MeshingChunkThread::MeshingChunkThread( MeshingChunkThreadManager* chunkMan, const int threadNum ) 
	: m_threadNum(threadNum)
{
	mChunkMan = chunkMan;
}

void MeshingChunkThread::run()
{
	try {
		doMeshStuff();
	} catch (...){
	}

	mChunkMan->mMeshManager->num_worker_threads_active->release(1);
}

void MeshingChunkThread::doMeshStuff()
{		
	mChunkMan->mutex->lock();
	OmMeshSource *mpCurrentMeshSource = mChunkMan->mpCurrentMeshSource;
	assert (mpCurrentMeshSource);
	OmMesher mesher(*mpCurrentMeshSource);
	assert (mpCurrentMeshSource);
	mChunkMan->mutex->unlock();

	//loop for each available segment value
	while (true) {

		//get next segment value
		SEGMENT_DATA_TYPE segment_value = mChunkMan->getNextSegmentValueToMesh();

		//if null, then no more values in set, so break loop
		if (NULL_SEGMENT_DATA == segment_value)
			break;
		
		//get mesh coordiante
		OmMipMeshCoord mesh_coord = OmMipMeshCoord( mChunkMan->mCurrentMipCoord, segment_value);

		//get alloc'd mesh
		OmMipMesh *p_mesh = mChunkMan->mMeshManager->mMipMeshManager->AllocMesh( mesh_coord);

		//build mesh data using thread's mesh source and seg value
		mesher.ExtractMesh(p_mesh, segment_value);

		//save mesh
		p_mesh->Save();

		//delete mesh
		delete p_mesh;		
	}
}
