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

	OmMeshSource *mpCurrentMeshSource;
	OmMipChunkCoord mCurrentMipCoord;

	QQueue< SEGMENT_DATA_TYPE > valuesToMesh;
	SEGMENT_DATA_TYPE getNextSegmentValueToMesh();

	QSemaphore* num_values_done;
	QMutex * mutex;

	MeshingManager* mMeshManager;

 private:
	OmMipChunkCoord mCoord;

};

#endif
