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
	OmSegmentCache * mSegmentCache;

	OmColor * mColorCache;
	int * mColorCacheFreshness;

	quint32 mSize;
	int freshness;
	int mSegCacheFreshness;

	void setup();

	OmColor getVoxelColorForView2d( const SEGMENT_DATA_TYPE val, 
					const bool showOnlySelectedSegments );


	int clamp(int c) {
		if (c > 255) {
			return 255;
		}
		return c;
	}
};

#endif
