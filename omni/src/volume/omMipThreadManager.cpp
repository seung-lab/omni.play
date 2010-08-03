#include "omMipThread.h"
#include "omMipThreadManager.h"
#include "omMipVolume.h"
#include "system/omLocalPreferences.h"
#include "volume/omMipChunkCoord.h"

OmMipThreadManager::OmMipThreadManager(OmMipVolume* pMipVolume, OmMipThread::ChunkType chunkType, int initLevel, bool buildEdited)
{
	mpMipVolume = pMipVolume;
	mChunkType = chunkType;
	mInitLevel = initLevel;
	mBuildEdited = buildEdited;
	mNumTotalThreads = OmLocalPreferences::numAllowedWorkerThreads();
}

/*
 *	Precalculates the number of chunks each thread gets. Pidgeon-holes according to
 *	thread number.
 */
int OmMipThreadManager::ChunksPerThread(int threadNum, int numTotalChunks)
{
	int quotient =  numTotalChunks / mNumTotalThreads;
	int remainder = numTotalChunks % mNumTotalThreads;

	if (threadNum < remainder){
		return quotient + 1;
	} else {
		return quotient;
	}
}

/*
 *	Creates and starts MipThreads using QThreadPool
 */
void OmMipThreadManager::SpawnThreads(int numTotalChunks)
{
	mMipThreadPool.setMaxThreadCount(mNumTotalThreads);
	for (int i = 0; i < mNumTotalThreads; i++){

		OmMipThread* thread = new OmMipThread(mpMipVolume,i,mChunkType,
						      ChunksPerThread(i,numTotalChunks));
		mMipThreads.append(thread);
		mMipThreadPool.start(thread);		
	}
}

/*
 *	Waits on threads in parent MipVolume and deletes them when done.
 */
void OmMipThreadManager::StopThreads()
{
	mMipThreadPool.waitForDone();
	mMipThreads.clear();
}


/*
 *	Distributes MipChunkCoords of ThreadChunks to threads for them to downsample.
 *	Assumes MipThreads are still alive.
 */
void OmMipThreadManager::DistributeThreadChunks()
{
	//Current thread number
	int threadNum = 0;

	if (mBuildEdited){
		//Loop through set of edited thread chunks
		//set <OmMipChunkCoord>::iterator itr;
		//for(itr = mpMipVolume->mEditedThreadChunks.begin(); itr!= mEditedThreadChunks.end; itr++){
		//	mMipThreads[threadNum]->AddEnqueuedThreadChunk(*itr);
		//	//Loop through threads
		//	threadNum++;
		//	if (threadNum == mNumTotalThreads){threadNum = 0;}
		//}
		
	} else {		
		//Dimensions of mip volume in thread chunks
		Vector3 < int > thread_coord_dims = mpMipVolume->MipLevelDimensionsInThreadChunks(mInitLevel);
		for (int z = 0; z < thread_coord_dims.z; ++z){
			for (int y = 0; y < thread_coord_dims.y; ++y){
				for (int x = 0; x < thread_coord_dims.x; ++x){ 	
					mMipThreads[threadNum]->EnqueueChunk(OmMipChunkCoord(mInitLevel, x, y, z));
					//Loop through threads
					threadNum++;
					if (threadNum == mNumTotalThreads){threadNum = 0;}
				}
			}
		}
	}
}

/*
 *	Distributes MipChunkCoords of MipChunks to threads. Should only be used by
 *	OmSegmentation to get/refresh direct data values.
 */
void OmMipThreadManager::DistributeMipChunks()
{
	//Current thread number
	int threadNum = 0;

	if (mBuildEdited){
		//Loop through set of edited thread chunks
		//set <OmMipChunkCoord>::iterator itr;
		//for(itr = mpMipVolume->mEditedChunks.begin(); itr!= mEditedChunks.end; itr++){
		//	mMipThreads[threadNum]->EnqueueChunk(*itr);
		//	//Loop through threads
		//	threadNum++;
		//	if (threadNum == mNumTotalThreads){threadNum = 0;}
		//}
		
	} else {		
		//Dimensions of mip volume in thread chunks
		for (int level = 0; level <= mpMipVolume->GetRootMipLevel(); ++level){
			Vector3 < int > mip_coord_dims = mpMipVolume->MipLevelDimensionsInMipChunks(level);
			for (int z = 0; z < mip_coord_dims.z; ++z){
				for (int y = 0; y < mip_coord_dims.y; ++y){
					for (int x = 0; x < mip_coord_dims.x; ++x){ 	
						mMipThreads[threadNum]->EnqueueChunk(OmMipChunkCoord(level, x, y, z));
						//Loop through threads
						threadNum++;
						if (threadNum == mNumTotalThreads){threadNum = 0;}
					}
				}
			}
		}
	}
}

void OmMipThreadManager::run()
{
	if (OmMipThread::THREAD_CHUNK == mChunkType){
		DistributeThreadChunks();
	} else if (OmMipThread::MIP_CHUNK == mChunkType){
		DistributeMipChunks();
	}
}
