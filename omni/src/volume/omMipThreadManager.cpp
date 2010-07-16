#include "omMipThread.h"
#include "omMipThreadManager.h"
#include "omMipVolume.h"
#include "system/omLocalPreferences.h"
#include "volume/omMipChunkCoord.h"

OmMipThreadManager::OmMipThreadManager(OmMipVolume* pMipVolume, bool buildEdited)
{
	mpMipVolume = pMipVolume;
	mBuildEdited = buildEdited;
	mNumTotalThreads = OmLocalPreferences::numAllowedWorkerThreads();
}

/*
 *	Precalculates the number of chunks each thread gets. Pidgeon-holes according to
 *	thread number.
 */
int OmMipThreadManager::ChunksPerThread(int threadNum, int numThreadChunksToMip)
{
	int quotient =  numThreadChunksToMip / mNumTotalThreads;
	int remainder = numThreadChunksToMip % mNumTotalThreads;

	if (threadNum < remainder){
		return quotient + 1;
	} else {
		return quotient;
	}
}

/*
 *	Creates and starts MipThreads using QThreadPool
 */
void OmMipThreadManager::SpawnThreads(int numThreadChunksToMip)
{
	mMipThreadPool.setMaxThreadCount(mNumTotalThreads);
	for (int i = 0; i < mNumTotalThreads; i++){

		OmMipThread* thread = new OmMipThread(mpMipVolume,i,
						      ChunksPerThread(i,numThreadChunksToMip));
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
		Vector3 < int > thread_coord_dims = mpMipVolume->MipVolumeDimensionsInThreadChunks();
		for (int z = 0; z < thread_coord_dims.z; ++z){
			for (int y = 0; y < thread_coord_dims.y; ++y){
				for (int x = 0; x < thread_coord_dims.x; ++x){ 	
					mMipThreads[threadNum]->AddEnqueuedThreadChunk(OmMipChunkCoord(0, x, y, z));
					//Loop through threads
					threadNum++;
					if (threadNum == mNumTotalThreads){threadNum = 0;}
				}
			}
		}
	}
}

void OmMipThreadManager::run()
{
	DistributeThreadChunks();
}
