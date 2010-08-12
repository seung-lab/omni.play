#ifndef OM_MESH_SEGMENT_LIST_THREAD_H
#define OM_MESH_SEGMENT_LIST_THREAD_H

#include "common/omCommon.h"
#include "volume/omMipChunkCoord.h"

#include <zi/threads>

class OmSegment;
class OmSegmentCache;

class OmMeshSegmentListThread : public zi::Runnable {
public:
	OmMeshSegmentListThread( OmMipChunkPtr p_chunk,
				 OmSegment * rootSeg,
				 const OmMipChunkCoord & chunkCoord,
				 boost::shared_ptr<OmSegmentCache> segmentCache,
				 const OmId segmentationID);

	void run();

private:
	OmMipChunkPtr mChunk;
	OmSegment *const mRootSeg;
	const OmMipChunkCoord mChunkCoord;
	boost::shared_ptr<OmSegmentCache> mSegmentCache;
	const OmId mSegmentationID;
};

#endif
