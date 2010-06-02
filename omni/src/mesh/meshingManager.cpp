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

int MeshingManager::getNumThreadOverride() 
{ 
	return numThreadOverride; 
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

int MeshingManager::numCoordsLeftToMesh()
{
	return mChunkCoords.size();
}

void MeshingManager::run()
{
	const int numChunksToProcess = mChunkCoords.size();
	printf("\tgoing to process %d chunks...\n", numChunksToProcess );

	const int maxNumChunkThreadManagers = getMaxAllowedNumberOfActiveChunks();
	const int maxNumberWorkerThreads = getMaxAllowedNumberOfWorkerThreads();

	num_chunk_threads_active = new QSemaphore( maxNumChunkThreadManagers );
	num_worker_threads_active = new QSemaphore( maxNumberWorkerThreads );

	QQueue<MeshingChunkThreadManager*> chunkThreads;
	while( !mChunkCoords.empty() ){
		OmMipChunkCoord coord = mChunkCoords.dequeue();
		num_chunk_threads_active->acquire(1);
		MeshingChunkThreadManager * chunkThread = new MeshingChunkThreadManager( this, coord );
		chunkThreads.enqueue(chunkThread);
		chunkThread->start();
        }
	
	foreach(MeshingChunkThreadManager* thread, chunkThreads){
		thread->wait();
		delete(thread);
	}

	delete num_chunk_threads_active;
	delete num_worker_threads_active;

	if( 0 != mFailedChunkCoords.size() ){
		printf("meshingManager ERROR: the following chunks failed to fully mesh:\n");
		foreach( OmMipChunkCoord coord, mFailedChunkCoords ) {
			printf("failed to mesh chunk %s\n", qPrintable( coord.getCoordsAsString() ) );
		}
	}
}
