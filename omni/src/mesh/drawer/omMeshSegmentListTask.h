#ifndef OM_MESH_SEGMENT_LIST_TASK_HPP
#define OM_MESH_SEGMENT_LIST_TASK_HPP

#include "volume/omVolumeTypes.hpp"
#include "zi/omThreads.h"

class OmSegment;
class OmMeshSegmentList;

class OmMeshSegmentListTask : public zi::runnable {
private:
	OmMipChunkPtr mChunk;
	OmSegment *const mRootSeg;
	OmMeshSegmentList *const rootSegLists_;

public:
	OmMeshSegmentListTask(OmMipChunkPtr p_chunk,
						  OmSegment* rootSeg,
						  OmMeshSegmentList* rootSegLists)
		: mChunk(p_chunk)
		, mRootSeg(rootSeg)
		, rootSegLists_(rootSegLists)
	{}

	void run();
};

#endif
