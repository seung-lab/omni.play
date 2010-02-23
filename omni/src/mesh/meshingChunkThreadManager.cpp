#include "meshingChunkThreadManager.h"
#include "common/omDebug.h"
#include "volume/omVolume.h"

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
	if( valuesToMesh.empty() ) {
		return NULL_SEGMENT_DATA;
	} 

	return valuesToMesh.dequeue();
}


void MeshingChunkThreadManager::run()
{
	shared_ptr < OmMipChunk > chunk = shared_ptr < OmMipChunk > ();
	OmVolume::GetSegmentation( mMeshManager->getSegmentationID() ).GetChunk( chunk, mCoord );

	foreach( SEGMENT_DATA_TYPE value, chunk->GetDirectDataValues() ) {
		if( NULL_SEGMENT_DATA == value ){
			continue;
		}
		valuesToMesh.enqueue( value );
	}

	const int totalNumValuesToMesh = valuesToMesh.size();

	if( totalNumValuesToMesh > 0 ){
		chunk->Open();

		mpCurrentMeshSource = new OmMeshSource();
		assert (mpCurrentMeshSource);
		mpCurrentMeshSource->Load(chunk);
		mpCurrentMeshSource->Copy(*mpCurrentMeshSource);
		mCurrentMipCoord = chunk->GetCoordinate();

		int num_threads_to_use = 2;
		if( totalNumValuesToMesh < 50 ){
			num_threads_to_use = 1;
		} else {
			printf("have %d values to mesh (lot!)\n", totalNumValuesToMesh );
		}
		num_threads_done = new QSemaphore(0);
		for( int i = 0; i < num_threads_to_use; i++ ){
			mMeshManager->num_worker_threads_active->acquire(1);
			MeshingChunkThread* thread = new MeshingChunkThread(this);
			thread->start();
		}

		num_threads_done->acquire( num_threads_to_use  );

		delete mpCurrentMeshSource;
		mpCurrentMeshSource = NULL;

		chunk->Close();
        }

	mMeshManager->num_chunks_done->release(1);
	mMeshManager->num_chunk_threads_active->release(1);
	printf("finished meashing chunk %d, %d, %d, %d\n", mCoord.Level, mCoord.Coordinate.x, 
	       mCoord.Coordinate.y, mCoord.Coordinate.z );
}
