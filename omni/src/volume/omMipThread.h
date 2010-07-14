#ifndef OM_MIP_THREAD_H
#define OM_MIP_THREAD_H

#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QQueue>

class OmMipVolume;
class OmMipChunkCoord;

class OmMipThread: public QThread
{
 public:
	OmMipThread(OmMipVolume* pMipVolume, const int threadNum, int numTotalThreadChunks);	
	void AddEnqueuedThreadChunk(OmMipChunkCoord mipCoord);
	void run();
 private:
	OmMipVolume* mpMipVolume;
	const int mThreadNum;

	QQueue <OmMipChunkCoord> mMipCoords;
	int mNumTotalThreadChunks;
	QSemaphore mNumEnqueuedThreadChunks;

	QMutex mutex;
};

#endif
