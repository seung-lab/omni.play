#include "segment/omSegmentCacheImplBatch.h"
#include "volume/omSegmentation.h"

OmSegmentCacheImplBatch::OmSegmentCacheImplBatch(OmSegmentation *segmentation, OmSegmentCache * cache )
	: OmSegmentCacheImpl( segmentation, cache )
{
	needToFlush = false;
}

OmSegmentCacheImplBatch::~OmSegmentCacheImplBatch()
{
	flushDirtySegments();
}

void OmSegmentCacheImplBatch::addToDirtySegmentList( OmSegment*)
{
	needToFlush = true;
}

void OmSegmentCacheImplBatch::flushDirtySegments()
{
	if( !needToFlush ){
		return;
	}

	printf("flushing all segment metadata; please wait...");
	foreach( OmSegment* seg, mSegIdToSegPtrHash ){
		Save( seg );
	}

	printf("done\n");
	needToFlush = false;
}
