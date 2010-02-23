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

	num_values_done = new QSemaphore(0);
	
	foreach( SEGMENT_DATA_TYPE value, chunk->GetDirectDataValues() ) {
		valuesToMesh.enqueue( value );
	}

	if( !valuesToMesh.empty() ) {
		chunk->Open();

		mpCurrentMeshSource = new OmMeshSource();
		mpCurrentMeshSource->Load(chunk);
		mpCurrentMeshSource->Copy(*mpCurrentMeshSource);

		mCurrentMipCoord = chunk->GetCoordinate();

		int num_threads_to_use = 5;
		for( int i = 0; i < num_threads_to_use; i++ ){
			mMeshManager->num_worker_threads_active->acquire(1);
			MeshingChunkThread* thread = new MeshingChunkThread(this);
			thread->start();
		}

		num_values_done->acquire( valuesToMesh.size() );

		delete mpCurrentMeshSource;
		mpCurrentMeshSource = NULL;

		chunk->Close();
        }

	mMeshManager->num_chunks_done->release(1);
	mMeshManager->num_chunk_threads_active->release(1);
	printf("finished meashing chunk %d, %d, %d, %d\n", mCoord.Level, mCoord.Coordinate.x, 
	       mCoord.Coordinate.y, mCoord.Coordinate.z );
}
