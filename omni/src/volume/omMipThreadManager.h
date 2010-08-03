#ifndef OM_MIP_THREAD_MANAGER_H
#define OM_MIP_THREAD_MANAGER_H

#include "omMipThread.h"

#include <QThreadPool>
#include <QThread>
#include <QList>

class OmMipVolume;

class OmMipThreadManager: public QThread
{
 public:
	OmMipThreadManager(OmMipVolume* pMipVolume, OmMipThread::ChunkType chunkType, int initLevel,  bool buildEdited);
	void run();
	void SpawnThreads(int numTotalChunks);
	void StopThreads();
 private:
	void DistributeThreadChunks();
	void DistributeMipChunks();
	int ChunksPerThread(int threadNum, int numTotalChunks);

	OmMipVolume* mpMipVolume;	
	OmMipThread::ChunkType mChunkType;

	QThreadPool mMipThreadPool;
	QList<OmMipThread*> mMipThreads;
	int mNumTotalThreads;
	int mInitLevel;
	bool mBuildEdited; //If set, build only edited granules instead of all granules
};

#endif
