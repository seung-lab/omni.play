#include "omMipThread.h"
#include "omMipVolume.h"

OmMipThread::OmMipThread(OmMipVolume *pMipVolume, const int threadNum, int numTotalThreadChunks)
	: mThreadNum(threadNum)
	, mNumTotalThreadChunks(numTotalThreadChunks)
{
	mpMipVolume = pMipVolume;
}


/*
 *	Adds a MipChunkCoord of a ThreadChunk to the queue to downsample.
 */
void OmMipThread::AddEnqueuedThreadChunk(OmMipChunkCoord mipCoord)
{
	mutex.lock();
	mMipCoords.enqueue(mipCoord);
	mutex.unlock();
	mNumEnqueuedThreadChunks.release();
}


/*
 *	Downsamples thread chunks in queue. Waits on queue if empty, but stops if num of
 *	thread chunks downsampled equals precalculated total.
 */
void OmMipThread::run()
{
	for (int i=0; i < mNumTotalThreadChunks; i++){

		mNumEnqueuedThreadChunks.acquire();		
		assert(!mMipCoords.isEmpty());
		mutex.lock();
		OmMipChunkCoord mipCoord = mMipCoords.dequeue();
		mutex.unlock();
		mpMipVolume->BuildThreadChunk(mipCoord,NULL);
	}
}
