#include "omMipThread.h"
#include "omMipVolume.h"

OmMipThread::OmMipThread(OmMipVolume *pMipVolume, const int threadNum, ChunkType chunkType, int numTotalChunks)
	: mThreadNum(threadNum)
{
	mpMipVolume = pMipVolume;
	mChunkType = chunkType;
	mNumTotalChunks = numTotalChunks;
}


/*
 *	Adds a MipChunkCoord of a chunk to the queue to downsample.
 */
void OmMipThread::EnqueueChunk(OmMipChunkCoord mipCoord)
{
	mutex.lock();
	mMipCoords.enqueue(mipCoord);
	mutex.unlock();
	mNumEnqueuedChunks.release();
}

/*
 *	For use in updating progress
 */
int OmMipThread::GetNumChunksDone()
{
	return mNumChunksDone;
}

/*
 *	Downsamples thread chunks in queue. Waits on queue if empty, but stops if num of
 *	thread chunks downsampled equals precalculated total.
 */
void OmMipThread::run()
{
	for (mNumChunksDone=0; mNumChunksDone < mNumTotalChunks; mNumChunksDone++){

		mNumEnqueuedChunks.acquire();		
		assert(!mMipCoords.isEmpty());
		mutex.lock();
		OmMipChunkCoord mipCoord = mMipCoords.dequeue();
		mutex.unlock();
		if (THREAD_CHUNK == mChunkType){
			mpMipVolume->BuildThreadChunk(mipCoord,NULL);
		} else if (MIP_CHUNK == mChunkType){
			mpMipVolume->BuildChunk(mipCoord);
		}
	}
}
