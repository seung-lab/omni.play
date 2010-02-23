#ifndef MESHING_MANAGER_H
#define MESHING_MANAGER_H

#include "system/omSystemTypes.h"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "mesh/omMipMeshManager.h"
#include <QQueue>
#include <QThread>
#include <QSemaphore>
using boost::shared_ptr;

class MeshingManager : public QThread
{
 public:
	MeshingManager( const OmId segmentationID, OmMipMeshManager * mipMeshManager );
	void addToQueue( const OmMipChunkCoord coord );
	void run();

	OmId getSegmentationID() { return mSegmentationID; }

	QSemaphore* num_chunks_done; // indicates when meshManager is done
	QSemaphore* num_chunk_threads_active; // limit number of chunks being worked on at once
	QSemaphore* num_worker_threads_active; // limit total number of threads running VTK meshing pipeline
	
	OmMipMeshManager * mMipMeshManager;

 private:
	const OmId mSegmentationID;
	QQueue< OmMipChunkCoord > mChunkCoords;


	int getMaxAllowedNumberOfActiveChunks();
	int getMaxAllowedNumberOfWorkerThreads();
};

#endif
