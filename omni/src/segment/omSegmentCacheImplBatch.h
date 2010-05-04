#ifndef OM_SEGMENT_CACHE_IMPL_BATCH_H
#define OM_SEGMENT_CACHE_IMPL_BATCH_H

#include "omSegmentCacheImpl.h"

class OmSegmentCacheImplBatch : public OmSegmentCacheImpl
{
public:
	OmSegmentCacheImplBatch(OmSegmentation * segmentation, OmSegmentCache * cache);
	~OmSegmentCacheImplBatch();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

 private:
	bool needToFlush;
};

#endif
