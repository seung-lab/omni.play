#include "omMipThread.h"
#include "omMipThreadManager.h"
#include "omMipVolume.h"
#include "system/omLocalPreferences.h"

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
 *	Creates threads in the parent MipVolume.
 */
void OmMipThreadManager::SpawnThreads(int numThreadChunksToMip)
{
	for (int i = 0; i < mNumTotalThreads; i++){

		OmMipThread* thread = new OmMipThread(mpMipVolume,i,
						      ChunksPerThread(i,numThreadChunksToMip));
		mpMipVolume->mMipThreads.append(thread);

	}
}

/*
 *	Starts threads in parent MipVolume.
 */
void OmMipThreadManager::StartThreads()
{
	foreach(OmMipThread* thread, mpMipVolume->mMipThreads){
		thread->start();
	}
}

/*
 *	Waits on threads in parent MipVolume and deletes them when done.
 */
void OmMipThreadManager::StopThreads()
{
	foreach(OmMipThread* thread, mpMipVolume->mMipThreads){
		thread->wait();
		delete(thread);
	}
	//Clear the list
	mpMipVolume->mMipThreads.clear();	
}


/*
 *	Distributes MipChunkCoords of ThreadChunks to threads for them to downsample.
 */
void OmMipThreadManager::DistributeThreadChunks()
{
	//Current thread number
	int threadNum = 0;

	if (mBuildEdited){
		//Loop through set of edited thread chunks
		//set <OmMipChunkCoord>::iterator itr;
		//for(itr = mpMipVolume->mEditedThreadChunks.begin(); itr!= mEditedThreadChunks.end; itr++){
		//	mpMipVolume->mMipThreads[threadNum]->AddEnqueuedThreadChunk(*itr);
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
 					//Assign mip granule to thread
					mpMipVolume->mMipThreads[threadNum]->AddEnqueuedThreadChunk(OmMipChunkCoord(0, x, y, z));
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
