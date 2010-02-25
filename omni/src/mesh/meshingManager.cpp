#include "meshingManager.h"
#include "meshingChunkThreadManager.h"

#include "system/omGarbage.h"
#include "system/omLocalPreferences.h"

MeshingManager::MeshingManager( const OmId segmentationID, OmMipMeshManager * mipMeshManager )
	: mSegmentationID(segmentationID)
{
	mMipMeshManager = mipMeshManager;
	assert(mMipMeshManager);
	onlyMeshModifiedValues = false;
	numThreadOverride = 0;
}

void MeshingManager::setNumThreads( const int numThreads )
{	
	numThreadOverride = numThreads;
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
	if( numThreadOverride > 0 ){
		return numThreadOverride;
	}

	return OmLocalPreferences::numAllowedWorkerThreads();
}

void MeshingManager::addToFailedQueue( OmMipChunkCoord coord )
{
	mFailedChunkCoords.enqueue( coord );
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

	if( 0 != mFailedChunkCoords.size() ){
		printf("meshingManager ERROR: the following chunks failed to fully mesh:\n");
		foreach( OmMipChunkCoord coord, mFailedChunkCoords ) {
			printf("failed to mesh chunk %s\n", qPrintable( coord.getCoordsAsString() ) );
		}
	}
}
