#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"

enum OmSegmentColorCacheType { Channel = 0, Segmentation, ChannelBreak, SegmentationBreak };

class OmSegmentCache;
class OmViewGroupState;
class OmSegment;

class OmSegmentColorizer 
{
 public:
	OmSegmentColorizer( OmSegmentCache *, const OmSegmentColorCacheType);
	void colorTile( SEGMENT_DATA_TYPE * imageData, const int size,
			unsigned char * data, OmViewGroupState * );

 private:
	OmSegmentCache * mSegmentCache;
	const OmSegmentColorCacheType mSccType;

	OmColor * mColorCache;
	int * mColorCacheFreshness;

	quint32 mSize;
	float mCurBreakThreshhold;
	float mPrevBreakThreshhold;

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

	bool isCacheElementValid( const SEGMENT_DATA_TYPE val, const int currentSegCacheFreshness ){
		if( currentSegCacheFreshness != mColorCacheFreshness[ val ] ){
			return false;
		}

		if( mCurBreakThreshhold != mPrevBreakThreshhold ){
			return false;
		}

		return true;
	}
};

#endif
