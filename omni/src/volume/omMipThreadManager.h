#ifndef OM_MIP_THREAD_MANAGER_H
#define OM_MIP_THREAD_MANAGER_H

#include <QThreadPool>
#include <QThread>
#include <QList>

class OmMipVolume;
class OmMipThread;

class OmMipThreadManager: public QThread
{
 public:
	OmMipThreadManager(OmMipVolume* pMipVolume, bool buildEdited);
	void run();
	void SpawnThreads(int numThreadChunksToMip);
	void StopThreads();
 private:
	void DistributeThreadChunks();
	int ChunksPerThread(int threadNum, int numThreadChunksToMip);

	OmMipVolume* mpMipVolume;
	QThreadPool mMipThreadPool;
	QList<OmMipThread*> mMipThreads;
	int mNumTotalThreads;
	bool mBuildEdited; //If set, build only edited granules instead of all granules
};

#endif
