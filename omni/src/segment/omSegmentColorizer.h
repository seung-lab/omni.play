#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"

#include "zi/omMutex.h"

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
	OmSegmentColorizer( boost::shared_ptr<OmSegmentCache>,
			    const OmSegmentColorCacheType,
			    const Vector2i& dims);

	boost::shared_ptr<OmColorRGBA> colorTile(boost::shared_ptr<uint32_t>);

	void setCurBreakThreshhold( const float t ) {
		mPrevBreakThreshhold = mCurBreakThreshhold;
		mCurBreakThreshhold = t;
	}

 private:
	zi::rwmutex mMapResizeMutex;

	boost::shared_ptr<OmSegmentCache> mSegmentCache;
	const OmSegmentColorCacheType mSccType;
	OmSegID mSize;
	float mCurBreakThreshhold;
	float mPrevBreakThreshhold;
	bool mAreThereAnySegmentsSelected;
 	const uint32_t mNumElements;
	int mCurSegCacheFreshness;

	std::vector<OmColorWithFreshness> mColorCache;

	void setup();

	OmColor getVoxelColorForView2d(const OmSegID val);

	inline int makeSelectedColor(const quint8 in_c) {
		static const double selectedSegColorFactor = 2.5;
		const int c = static_cast<int>((double)in_c * selectedSegColorFactor);
		if (c > 255) {
			return 255;
		}
		return c;
	}

	void doColorTile(uint32_t*, OmColorRGBA*);

	struct segment_colorizer_mutex_pool_tag;
};

#endif
