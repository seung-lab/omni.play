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

	time_t time_start;
	time_t time_end;
	double time_dif;

	printf("flushing all segment metadata; please wait...");
	time(&time_start);

	foreach( OmSegment* seg, mSegIdToSegPtrHash ){
		Save( seg );
	}

	time(&time_end);
	time_dif = difftime(time_end, time_start);

	printf("done (%.2lf secs)\n", time_dif);

	needToFlush = false;
}
