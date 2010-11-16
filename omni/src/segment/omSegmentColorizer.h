#ifndef OM_SEGEMNT_COLORIZER_H
#define OM_SEGEMNT_COLORIZER_H

#include "common/omCommon.h"
#include "zi/omMutex.h"

class OmSegment;
class OmSegmentCache;
class OmViewGroupState;

class OmSegmentColorizer {
public:
	OmSegmentColorizer( OmSegmentCache*,
						const OmSegmentColorCacheType,
						const Vector2i& dims);

	boost::shared_ptr<OmColorRGBA> ColorTile(uint32_t const*const imageData);

private:
	struct OmColorWithFreshness
	{
		OmColor color;
		int freshness;
	};

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

	static const std::vector<uint8_t> selectedColorLookup_;

	static inline OmColor makeSelectedColor(const OmColor& color)
	{
		const OmColor ret = {selectedColorLookup_[color.red],
							 selectedColorLookup_[color.green],
							 selectedColorLookup_[color.blue]};
		return ret;
	}

	static inline uint8_t makeSelectedColor(const uint8_t val)
	{
		static const double selectedSegColorFactor = 2.5;

		return val > 101 ?
			255 :
			static_cast<uint8_t>(val * selectedSegColorFactor);
	}

	static std::vector<uint8_t> makeLookupTable()
	{
		std::vector<uint8_t> ret(256, 0);
		for(int i = 0; i < 256; ++i){
			ret[i] = makeSelectedColor(i);
		}
		return ret;
	}

	void doColorTile(uint32_t const*, OmColorRGBA*);

	struct colorizer_mutex_pool_tag;
	typedef zi::spinlock::pool<colorizer_mutex_pool_tag>::guard mutex_guard_t;

	friend class SegmentTests1;
};

#endif
