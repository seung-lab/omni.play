#include "omSegmentationChunkCoord.h"

OmSegmentationChunkCoord::OmSegmentationChunkCoord( OmId segmentationID, int level, int x, int y, int z )
	: OmMipChunkCoord( level, x, y, z), mSegmentationID( segmentationID )
{
}

OmId OmSegmentationChunkCoord::getSegmentationID()
{
	return mSegmentationID;
}
