#ifndef OM_MIP_THREAD_MANAGER_H
#define OM_MIP_THREAD_MANAGER_H

#include <QThread>
#include <QQueue>

class OmMipVolume;

class OmMipThreadManager: public QThread
{
 public:
	OmMipThreadManager(OmMipVolume* pMipVolume, bool buildEdited);
	void run();
	void SpawnThreads(int numThreadChunksToMip);
	void StartThreads();
	void StopThreads();
 private:
	void DistributeThreadChunks();
	int ChunksPerThread(int threadNum, int numThreadChunksToMip);

	OmMipVolume* mpMipVolume;
	int mNumTotalThreads;
	bool mBuildEdited; //If set, build only edited granules instead of all granules
};

#endif
