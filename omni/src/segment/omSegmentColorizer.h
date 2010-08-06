#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"

#include <QMutex>
#include <QReadWriteLock>

static const double selectedSegmentColorMultiFactor = 2.5;

typedef struct {
	OmColor color;
	int freshness;
} OmColorWithFreshness;

class OmSegmentCache;
class OmViewGroupState;
class OmSegment;

class OmSegmentColorizer
{
 public:
	OmSegmentColorizer( OmSegmentCache *, const OmSegmentColorCacheType, const bool);
	~OmSegmentColorizer();

	void colorTile( OmSegID * imageData, const int size,
			unsigned char * data );

	void setCurBreakThreshhold( const float t ) {
		mPrevBreakThreshhold = mCurBreakThreshhold;
		mCurBreakThreshhold = t;
	}

 private:
	mutable QMutex mColorUpdateMutex;
	mutable QReadWriteLock mMapResizeMutex;

	OmSegmentCache * mSegmentCache;
	const OmSegmentColorCacheType mSccType;
	quint32 mSize;
	float mCurBreakThreshhold;
	float mPrevBreakThreshhold;
	const bool mIsSegmentation;
	bool mAreThereAnySegmentsSelected;

	std::vector<OmColorWithFreshness> mColorCache;

	void setup();

	OmColor getVoxelColorForView2d( const OmSegID val );

	inline int makeSelectedColor(const quint8 in_c ) {
		const int c = static_cast<int>((double)in_c * selectedSegmentColorMultiFactor);
		if (c > 255) {
			return 255;
		}
		return c;
	}

	inline bool isCacheElementValid( const OmSegID val, const int currentSegCacheFreshness ){
		if( currentSegCacheFreshness != mColorCache[val].freshness ){
			return false;
		}
		if( mCurBreakThreshhold != mPrevBreakThreshhold ){
			return false;
		}
		return true;
	}
};

#endif
