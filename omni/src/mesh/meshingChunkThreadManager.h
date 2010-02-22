#ifndef MESHING_CHUNK_THREAD_MANAGER_H
#define MESHING_CHUNK_THREAD_MANAGER_H

#include "meshingManager.h"
#include "omMesher.h"

class MeshingChunkThreadManager : public QThread
{
 public:
	MeshingChunkThreadManager( MeshingManager* meshManager, OmMipChunkCoord coord );
	void run();

	SegmentDataSet mCurrentSegmentDataSet;
	OmMeshSource *mpCurrentMeshSource;
	OmMipChunkCoord mCurrentMipCoord;

 private:
	MeshingManager* mMeshManager;
	OmMipChunkCoord mCoord;

	QSemaphore* num_values_done;
	QSemaphore* num_value_threads_active;

};

#endif
