#ifndef OM_MESH_SEGMENT_LIST_TASK_HPP
#define OM_MESH_SEGMENT_LIST_TASK_HPP

#include "chunks/omChunkTypes.hpp"
#include "zi/omThreads.h"

class OmSegment;
class OmMeshSegmentList;
class OmSegmentation;

class OmMeshSegmentListTask : public zi::runnable {
private:
	OmSegChunkPtr mChunk;
	OmSegment *const mRootSeg;
	OmMeshSegmentList *const rootSegLists_;
	OmSegmentation *const segmentation_;

public:
	OmMeshSegmentListTask(OmSegChunkPtr p_chunk,
						  OmSegment* rootSeg,
						  OmMeshSegmentList* rootSegLists,
						  OmSegmentation* segmentation)
		: mChunk(p_chunk)
		, mRootSeg(rootSeg)
		, rootSegLists_(rootSegLists)
		, segmentation_(segmentation)
	{}

	void run();
};

#endif
