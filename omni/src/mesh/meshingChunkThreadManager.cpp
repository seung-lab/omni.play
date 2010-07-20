#include "project/omProject.h"
#include "meshingChunkThreadManager.h"
#include "common/omDebug.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

MeshingChunkThreadManager::MeshingChunkThreadManager( MeshingManager* meshManager, OmMipChunkCoord coord ) 
{
	mMeshManager = meshManager;
	mMipCoord = coord;
	//mCoord = OmMipChunkCoord(0,coord.getCoordinateX(), coord.getCoordinateY(), coord.getCoordinateZ());
	mCoord = coord;
	mpCurrentMeshSource = NULL;
}

unsigned int MeshingChunkThreadManager::getNumSegmentValuesLeftToMesh()
{
	QMutexLocker locker(&mutex);
	return valuesToMesh.size();
}

OmSegID MeshingChunkThreadManager::getNextSegmentValueToMesh()
{
	QMutexLocker locker(&mutex);

	if( valuesToMesh.empty() ) {
		return NULL_SEGMENT_VALUE;
	} 

	//pop value
	OmSegID segment_value = *valuesToMesh.begin();
	valuesToMesh.remove(segment_value);
	assert(NULL_SEGMENT_VALUE != segment_value);

	unsigned int leftToDo = totalNumValuesToMesh - valuesToMesh.size();
	debug("meshverbose", "MeshingChunkThreadManager: in %s: for chunk (%s), %d of %d values given out\n", 
	      __FUNCTION__, qPrintable( mCoord.getCoordsAsString()), leftToDo, totalNumValuesToMesh );

	return segment_value;
}

void MeshingChunkThreadManager::setupValuesToMesh( QExplicitlySharedDataPointer < OmMipChunk > chunk )
{
	if( mMeshManager->shouldIonlyMeshModifiedValues() ){
		valuesToMesh = chunk->GetModifiedVoxelValues();
	} else {
		valuesToMesh = chunk->GetDirectDataValues();
	}

	valuesToMesh.remove(NULL_SEGMENT_VALUE);

	totalNumValuesToMesh = valuesToMesh.size();

	if( totalNumValuesToMesh > 50000 ){
		printf("warning: meshing chunk (%s) with %d values...\n", qPrintable( mCoord.getCoordsAsString()), totalNumValuesToMesh );
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
		num_threads_to_use = static_cast<int>(totalNumValuesToMesh / 50.0 + 1);
		if (num_threads_to_use > maxNumWorkerThreads ) {
			num_threads_to_use = maxNumWorkerThreads;
		}
	}

	return num_threads_to_use;
}

void MeshingChunkThreadManager::run()
{
	QExplicitlySharedDataPointer < OmMipChunk > chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	OmProject::GetSegmentation( mMeshManager->getSegmentationID() ).GetChunk( chunk, mCoord );

	setupValuesToMesh( chunk );

	if( totalNumValuesToMesh > 0 ){
		getDataAndSpawnWorkerThread( chunk );
        }

	chunk = QExplicitlySharedDataPointer < OmMipChunk > ();

	mMeshManager->num_chunk_threads_active->release(1);
	printf("\tfinished meashing chunk %s; %d left\n", qPrintable( mCoord.getCoordsAsString()),
	       mMeshManager->numCoordsLeftToMesh() + 
	       mMeshManager->getMaxAllowedNumberOfActiveChunks() -
	       mMeshManager->num_chunk_threads_active->available() );
}

void MeshingChunkThreadManager::getDataAndSpawnWorkerThread( QExplicitlySharedDataPointer < OmMipChunk > chunk )
{
	chunk->Open();

	mpCurrentMeshSource = new OmMeshSource();
	assert (mpCurrentMeshSource);
	mpCurrentMeshSource->Load(chunk);
	mpCurrentMeshSource->Copy(*mpCurrentMeshSource);
	mCurrentMipCoord = chunk->GetCoordinate();

	int num_threads_to_use = numberOfThreadsToUseForThisChunk( totalNumValuesToMesh );
	
	QQueue<MeshingChunkThread*> threads;
	for( int i = 0; i < num_threads_to_use; i++ ){
		mMeshManager->num_worker_threads_active->acquire(1);
		if( 0 == getNumSegmentValuesLeftToMesh() ) {
			mMeshManager->num_worker_threads_active->release(1);
			break;
		} 
		MeshingChunkThread* thread = new MeshingChunkThread(this, i);
		threads.enqueue(thread);
		thread->start();
	}

	foreach(MeshingChunkThread* thread, threads){
		thread->wait();
		delete(thread);
	}

	if( 0 != valuesToMesh.size() ){
		mMeshManager->addToFailedQueue( mCoord );
	}

	delete mpCurrentMeshSource;
	mpCurrentMeshSource = NULL;
}
