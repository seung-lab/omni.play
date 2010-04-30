#ifndef OM_SEGMENTATION_CHUNK_COORD_H
#define OM_SEGMENTATION_CHUNK_COORD_H

#include "omMipChunkCoord.h"

class OmSegmentationChunkCoord : public OmMipChunkCoord
{
 public:
	OmSegmentationChunkCoord( OmId, int level, int, int, int );
	OmId getSegmentationID();

 private:
	const OmId mSegmentationID;
};

#endif
