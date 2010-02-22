#include "meshingChunkThreadManager.h"
#include "common/omDebug.h"
#include "volume/omVolume.h"

MeshingChunkThreadManager::MeshingChunkThreadManager( MeshingManager* meshManager, OmMipChunkCoord coord ) 
{
	mMeshManager = meshManager;
	mCoord = coord;
	mpCurrentMeshSource = NULL;
}

void MeshingChunkThreadManager::run()
{
	shared_ptr < OmMipChunk > chunk = shared_ptr < OmMipChunk > ();
	OmVolume::GetSegmentation( mMeshManager->getSegmentationID() ).GetChunk( chunk, mCoord );

	if( 0 != chunk->GetDirectDataValues().size() ){
		chunk->Open();

		mCurrentSegmentDataSet = chunk->GetDirectDataValues();
		mCurrentSegmentDataSet.erase(NULL_SEGMENT_DATA);

		//mpCurrentMeshSource = new OmMeshSource();
		//mpCurrentMeshSource->Load(chunk);
		//mpCurrentMeshSource->Copy(*mpCurrentMeshSource);

		mCurrentMipCoord = chunk->GetCoordinate();

		// TODO: thread off!

		delete mpCurrentMeshSource;
		mpCurrentMeshSource = NULL;

		chunk->Close();
        }

	mMeshManager->num_chunks_done->release(1);
	printf("finished meashing chunk %d, %d, %d, %d\n", mCoord.Level, mCoord.Coordinate.x, 
	       mCoord.Coordinate.y, mCoord.Coordinate.z );
}
