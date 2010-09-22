#ifndef OM_MESH_SEGMENT_LIST_THREAD_H
#define OM_MESH_SEGMENT_LIST_THREAD_H

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/threads>

class OmSegment;

class OmMeshSegmentListThread : public zi::Runnable {
public:
	OmMeshSegmentListThread(OmMipChunkPtr, OmSegment *);
	void run();

private:
	OmMipChunkPtr mChunk;
	OmSegment *const mRootSeg;
};

#endif
