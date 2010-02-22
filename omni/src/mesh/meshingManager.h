#ifndef MESHING_MANAGER_H
#define MESHING_MANAGER_H

#include "system/omSystemTypes.h"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include <QQueue>
#include <QThread>
#include <QSemaphore>
using boost::shared_ptr;

class MeshingManager : public QThread
{
 public:
	MeshingManager( const OmId segmentationID );
	void addToQueue( const OmMipChunkCoord coord );
	void run();

	OmId getSegmentationID() { return mSegmentationID; }

	QSemaphore* num_chunks_done; // indicates when meshManager is done

	QSemaphore* num_worker_threads_active; // limit total number of threads running VTK meshing pipeline
	
 private:
	const OmId mSegmentationID;
	QQueue< OmMipChunkCoord > mChunkCoords;

	QSemaphore* num_chunk_threads_active; // limit number of chunks being worked on at once
};

#endif
