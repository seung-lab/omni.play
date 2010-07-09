#ifndef OM_MESH_SEGMENT_LIST_THREAD_H
#define OM_MESH_SEGMENT_LIST_THREAD_H

#include "common/omCommon.h"
#include "volume/omMipChunkPtr.h"
#include "volume/omMipChunkCoord.h"

#include <QThread>

class OmSegment;
class OmSegmentCache;

class OmMeshSegmentListThread : public QThread {
 public:
	OmMeshSegmentListThread( OmMipChunkPtr p_chunk, 
				 OmSegment * rootSeg,
				 const OmMipChunkCoord & chunkCoord,
				 OmSegmentCache * segmentCache,
				 const OmId segmentationID);

	void run();

 private:
	OmMipChunkPtr mChunk;
	OmSegment *const mRootSeg;
	const OmMipChunkCoord mChunkCoord;
	OmSegmentCache *const mSegmentCache;
	const OmId mSegmentationID;
};

#endif
