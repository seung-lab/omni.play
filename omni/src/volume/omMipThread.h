#ifndef OM_MIP_THREAD_H
#define OM_MIP_THREAD_H

#include "volume/omMipChunkCoord.h"

#include <QRunnable>
#include <QSemaphore>
#include <QMutex>
#include <QQueue>

class OmMipVolume;

class OmMipThread: public QRunnable
{
 public:
	enum ChunkType
	{
		MIP_CHUNK,
		THREAD_CHUNK
	};

	OmMipThread(OmMipVolume* pMipVolume, const int threadNum, ChunkType chunkType, int numTotalChunks);
	int GetNumChunksDone();
	void EnqueueChunk(OmMipChunkCoord mipCoord);
	void run();

 private:
	OmMipVolume* mpMipVolume;
	const int mThreadNum;

	ChunkType mChunkType;

	QQueue<OmMipChunkCoord> mMipCoords;
	int mNumChunksDone;
	int mNumTotalChunks;
	
	mutable QSemaphore mNumEnqueuedChunks;
	mutable QMutex mutex;
};

#endif
