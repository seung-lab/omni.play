#ifndef MESHING_CHUNK_THREAD_MANAGER_H
#define MESHING_CHUNK_THREAD_MANAGER_H

#include "meshingManager.h"
#include "meshingChunkThread.h"
#include "omMesher.h"
#include <QMutex>

class MeshingChunkThreadManager : public QThread
{
 public:
	MeshingChunkThreadManager( MeshingManager* meshManager, OmMipChunkCoord coord );
	void run();

	QMutex * mutex;
	MeshingManager* mMeshManager;
	OmMeshSource *mpCurrentMeshSource;
	OmMipChunkCoord mCurrentMipCoord;

	OmSegID getNextSegmentValueToMesh();
	unsigned int getNumSegmentValuesLeftToMesh();

 private:
	OmSegIDs valuesToMesh;
	OmMipChunkCoord mCoord;
	OmMipChunkCoord mMipCoord;
	void setupValuesToMesh( QExplicitlySharedDataPointer < OmMipChunk > chunk );
	int numberOfThreadsToUseForThisChunk( const int totalNumValuesToMesh );
	unsigned int totalNumValuesToMesh;
	void getDataAndSpawnWorkerThread( QExplicitlySharedDataPointer < OmMipChunk > chunk );
};

#endif
