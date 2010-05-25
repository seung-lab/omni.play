#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"
#include <QMutex> 
enum OmSegmentColorCacheType { Channel = 0, Segmentation, ChannelBreak, SegmentationBreak };

class OmSegmentCache;
class OmViewGroupState;
class OmSegment;

static double selectedSegmentColorMultiFactor = 2.5;

class OmSegmentColorizer 
{
 public:
	OmSegmentColorizer( OmSegmentCache *, const OmSegmentColorCacheType);
	void colorTile( OmSegID * imageData, const int size,
			unsigned char * data, OmViewGroupState * );

 private:
	QMutex mMutex;

	OmSegmentCache * mSegmentCache;
	const OmSegmentColorCacheType mSccType;

	OmColor * mColorCache;
	int * mColorCacheFreshness;

	quint32 mSize;
	float mCurBreakThreshhold;
	float mPrevBreakThreshhold;

	void setup();

	OmColor getVoxelColorForView2d( const OmSegID & val, 
					const bool & showOnlySelectedSegments );

	int makeSelectedColor(const quint8 & in_c ) {
		const int c = (double)in_c * selectedSegmentColorMultiFactor;
		if (c > 255) {
			return 255;
		}
		return c;
	}

	bool isCacheElementValid( const OmSegID & val, const int & currentSegCacheFreshness );
};

#endif
