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
	OmSegmentColorizer( OmSegmentCache*,
						const OmSegmentColorCacheType,
						const Vector2i& dims);

	boost::shared_ptr<OmColorRGBA> ColorTile(uint32_t const*const imageData);

private:
	zi::rwmutex mMapResizeMutex;

	OmSegmentCache* mSegmentCache;
	const OmSegmentColorCacheType mSccType;
	OmSegID mSize;
	bool mAreThereAnySegmentsSelected;
 	const uint32_t mNumElements;
	int mCurSegCacheFreshness;

	std::vector<OmColorWithFreshness> mColorCache;

	void setup();

	OmColor getVoxelColorForView2d(const OmSegID val);

	static inline OmColor makeSelectedColor(const OmColor& color)
	{
		static const double selectedSegColorFactor = 2.5;

		const OmColor ret =
			{(color.red   > 101 ? 255 : color.red   * selectedSegColorFactor),
			 (color.green > 101 ? 255 : color.green * selectedSegColorFactor),
			 (color.blue  > 101 ? 255 : color.blue  * selectedSegColorFactor)
			};
		return ret;
	}

	void doColorTile(uint32_t const*, OmColorRGBA*);

	struct colorizer_mutex_pool_tag;
	typedef zi::spinlock::pool<colorizer_mutex_pool_tag>::guard mutex_guard_t;

	friend class SegmentTests;
};

#endif
