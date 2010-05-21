#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"

enum OmSegmentColorCacheType { Channel = 0, Segmentation, ChannelBreak, SegmentationBreak };

class OmSegmentCache;

class OmSegmentColorizer 
{
 public:
	OmSegmentColorizer( OmSegmentCache *, const OmSegmentColorCacheType);
	void colorTile( SEGMENT_DATA_TYPE * imageData, const int size,
			unsigned char * data );

 private:
	OmSegmentCache * mSegmentCache;
	const OmSegmentColorCacheType mSccType;

	OmColor * mColorCache;
	int * mColorCacheFreshness;

	quint32 mSize;
	int freshness;

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
