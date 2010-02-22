#include "meshingManager.h"
#include "meshingChunkThreadManager.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

MeshingManager::MeshingManager( const OmId segmentationID )
	: mSegmentationID(segmentationID)
{
}

void MeshingManager::addToQueue( const OmMipChunkCoord coord )
{
	mChunkCoords.enqueue( coord );
}

void MeshingManager::run()
{
	const int numChunksToProcess = mChunkCoords.size();
	const int maxNumChunkThreadManagers = 5; // OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT

	const int maxNumberWorkerThreads = OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT);

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
