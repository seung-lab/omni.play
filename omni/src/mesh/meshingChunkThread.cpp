#include "meshingChunkThread.h"
#include "meshingChunkThreadManager.h"

MeshingChunkThread::MeshingChunkThread( MeshingChunkThreadManager* chunkMan )
{
	mChunkMan = chunkMan;
}

void MeshingChunkThread::run()
{
	//OmMesher mesher(*mpCurrentMeshSource);

	//loop for each available segment value
	while (true) {

		//get next segment value
		SEGMENT_DATA_TYPE segment_value = mChunkMan->getNextSegmentValueToMesh();

		//if null, then no more values in set, so break loop
		if (NULL_SEGMENT_DATA == segment_value)
			break;
		
		/*
		  get the mutex

		//get mesh coordiante
		OmMipMeshCoord mesh_coord = OmMipMeshCoord(mCurrentMipCoord, segment_value);
		debug("mesher1", "OmMipChunkMesher::BuildMeshesLoop(): thread:(%i) \n", thread_index);

		//get alloc'd mesh
		OmMipMesh *p_mesh = mpMipMeshManager->AllocMesh(mesh_coord);

		//build mesh data using thread's mesh source and seg value
		mesher.ExtractMesh(p_mesh, segment_value);

		//save mesh
		p_mesh->Save();

		//delete mesh
		delete p_mesh;

		*/

		mChunkMan->num_values_done->release(1);
	}

	mChunkMan->mMeshManager->num_worker_threads_active->release(1);
}
