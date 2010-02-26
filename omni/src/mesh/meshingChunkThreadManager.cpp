#include "meshingChunkThreadManager.h"
#include "common/omDebug.h"
#include "volume/omVolume.h"
#include "segment/omSegmentTypes.h"

MeshingChunkThreadManager::MeshingChunkThreadManager( MeshingManager* meshManager, OmMipChunkCoord coord ) 
{
	mMeshManager = meshManager;
	mCoord = coord;
	mpCurrentMeshSource = NULL;

	mutex = new QMutex();
}

SEGMENT_DATA_TYPE MeshingChunkThreadManager::getNextSegmentValueToMesh()
{
	QMutexLocker locker( mutex );

	if( 0 == valuesToMesh.size() ) {
		return NULL_SEGMENT_DATA;
	} 

	//pop value
	SEGMENT_DATA_TYPE segment_value = *valuesToMesh.begin();
	valuesToMesh.erase(segment_value);
	assert(NULL_SEGMENT_DATA != segment_value);
	return segment_value;
}

void MeshingChunkThreadManager::setupValuesToMesh( shared_ptr < OmMipChunk > chunk )
{
	if( mMeshManager->shouldIonlyMeshModifiedValues() ){
		valuesToMesh = chunk->GetModifiedVoxelValues();
	} else {
		valuesToMesh = chunk->GetDirectDataValues();
	}

	valuesToMesh.erase(NULL_SEGMENT_DATA);

	totalNumValuesToMesh = valuesToMesh.size();

	if( totalNumValuesToMesh > 50000 ){
		printf("warning: meshing a chunk with %d values...\n", totalNumValuesToMesh );
	}
}

int MeshingChunkThreadManager::numberOfThreadsToUseForThisChunk( const int totalNumValuesToMesh )
{
	const int numThreadOverride = mMeshManager->getNumThreadOverride();
	if (numThreadOverride > 0 ){
		return numThreadOverride;
	}

	const int maxNumWorkerThreads = mMeshManager->getMaxAllowedNumberOfWorkerThreads();
	int num_threads_to_use = 1;
	if( totalNumValuesToMesh > 50 ){
		num_threads_to_use = totalNumValuesToMesh / 50.0 + 1;
		if (num_threads_to_use > maxNumWorkerThreads ) {
			num_threads_to_use = maxNumWorkerThreads;
		}
	}

	return num_threads_to_use;
}

void MeshingChunkThreadManager::run()
{
	shared_ptr < OmMipChunk > chunk = shared_ptr < OmMipChunk > ();
	OmVolume::GetSegmentation( mMeshManager->getSegmentationID() ).GetChunk( chunk, mCoord );

	setupValuesToMesh( chunk );

	if( totalNumValuesToMesh > 0 ){
		getDataAndSpawnWorkerThread( chunk );
        }

	mMeshManager->num_chunks_done->release(1);
	mMeshManager->num_chunk_threads_active->release(1);
	//	printf("finished meashing chunk %s\n", qPrintable( mCoord.getCoordsAsString()));
}

void MeshingChunkThreadManager::getDataAndSpawnWorkerThread( shared_ptr < OmMipChunk > chunk )
{
	chunk->Open();

	mpCurrentMeshSource = new OmMeshSource();
	assert (mpCurrentMeshSource);
	mpCurrentMeshSource->Load(chunk);
	mpCurrentMeshSource->Copy(*mpCurrentMeshSource);
	mCurrentMipCoord = chunk->GetCoordinate();

	int num_threads_to_use = numberOfThreadsToUseForThisChunk( totalNumValuesToMesh );

	num_threads_done = new QSemaphore(0);
	for( int i = 0; i < num_threads_to_use; i++ ){
		mMeshManager->num_worker_threads_active->acquire(1);
		MeshingChunkThread* thread = new MeshingChunkThread(this);
		thread->start();
	}

	num_threads_done->acquire( num_threads_to_use  );

	if( 0 != valuesToMesh.size() ){
		mMeshManager->addToFailedQueue( mCoord );
	}

	delete mpCurrentMeshSource;
	mpCurrentMeshSource = NULL;

	chunk = shared_ptr < OmMipChunk > ();
}
