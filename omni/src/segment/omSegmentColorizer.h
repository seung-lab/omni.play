#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"

class OmSegmentCache;

class OmSegmentColorizer 
{
 public:
	OmSegmentColorizer( OmSegmentCache *);
	void colorTile( SEGMENT_DATA_TYPE * imageData, const int size,
			const bool isSegmentation, unsigned char * data );

 private:
	OmSegmentCache * mCache;

};

#endif
