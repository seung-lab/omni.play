#include "meshingManager.h"
#include "meshingChunkThreadManager.h"

#include "system/omGarbage.h"
#include "system/omNumCores.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

MeshingManager::MeshingManager( const OmId segmentationID, OmMipMeshManager * mipMeshManager )
	: mSegmentationID(segmentationID)
{
	mMipMeshManager = mipMeshManager;
	assert(mMipMeshManager);
	onlyMeshModifiedValues = false;
}

void MeshingManager::addToQueue( const OmMipChunkCoord coord )
{
	mChunkCoords.enqueue( coord );
}

void MeshingManager::setToOnlyMeshModifiedValues()
{
	onlyMeshModifiedValues = true;
}

bool MeshingManager::shouldIonlyMeshModifiedValues()
{
	return onlyMeshModifiedValues;
}

int MeshingManager::getMaxAllowedNumberOfActiveChunks()
{
	return getMaxAllowedNumberOfWorkerThreads();
}

int MeshingManager::getMaxAllowedNumberOfWorkerThreads()
{
	if (OmGarbage::GetParallel()) {
		const int numCoresRaw = (int)OmNumCores::get_num_cores();
		int numCores = numCoresRaw - 1;
		if( 1 == numCoresRaw ){
			numCores = 1;
		}
		return numCores;
	} else {
		return OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT);
	}
}

void MeshingManager::run()
{
	const int numChunksToProcess = mChunkCoords.size();
	printf("going to process %d chunks...\n", numChunksToProcess );
	const int maxNumChunkThreadManagers = getMaxAllowedNumberOfActiveChunks();
	const int maxNumberWorkerThreads = getMaxAllowedNumberOfWorkerThreads();

	num_chunks_done = new QSemaphore(0);
	num_chunk_threads_active = new QSemaphore( maxNumChunkThreadManagers );
	num_worker_threads_active = new QSemaphore( maxNumberWorkerThreads );

	foreach( OmMipChunkCoord coord, mChunkCoords ) {
		num_chunk_threads_active->acquire(1);
		MeshingChunkThreadManager * chunkThread = new MeshingChunkThreadManager( this, coord );
		chunkThread->start();
        }
	
	num_chunks_done->acquire( numChunksToProcess );

	delete num_chunks_done;
	delete num_chunk_threads_active;
	delete num_worker_threads_active;
}
