#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"
#include <QMutex> 

static const double selectedSegmentColorMultiFactor = 2.5;

class OmSegmentCache;
class OmViewGroupState;
class OmSegment;

class OmSegmentColorizer 
{
 public:
	OmSegmentColorizer( OmSegmentCache *, const OmSegmentColorCacheType);
	~OmSegmentColorizer();

	void colorTile( OmSegID * imageData, const int size,
			unsigned char * data );

	void setCurBreakThreshhold( const float t ) {
		mPrevBreakThreshhold = mCurBreakThreshhold;
		mCurBreakThreshhold = t;
	}

 private:
	QMutex mMutex;

	OmSegmentCache * mSegmentCache;
	const OmSegmentColorCacheType mSccType;

	std::vector<OmColor> mColorCache;
	std::vector<int> mColorCacheFreshness;

	quint32 mSize;
	float mCurBreakThreshhold;
	float mPrevBreakThreshhold;

	void setup();

	OmColor getVoxelColorForView2d( const OmSegID val, 
					const bool showOnlySelectedSegments );

	int makeSelectedColor(const quint8 in_c ) {
		const int c = static_cast<int>((double)in_c * selectedSegmentColorMultiFactor);
		if (c > 255) {
			return 255;
		}
		return c;
	}

	bool isCacheElementValid( const OmSegID val, const int currentSegCacheFreshness ){
		if( currentSegCacheFreshness != mColorCacheFreshness[val] ){
			return false;
		}
		if( mCurBreakThreshhold != mPrevBreakThreshhold ){
			return false;
		}
		return true;
	}
};

#endif
