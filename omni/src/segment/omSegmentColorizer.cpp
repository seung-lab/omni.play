#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/cache/omCacheManager.h"
#include "utility/omSmartPtr.hpp"

static const OmColor blackColor = {0, 0, 0};

const std::vector<uint8_t> OmSegmentColorizer::selectedColorLookup_ =
	OmSegmentColorizer::makeLookupTable();

OmSegmentColorizer::OmSegmentColorizer( OmSegmentCache* cache,
					const OmSegmentColorCacheType sccType,
					const Vector2i& dims)
	: mSegmentCache(cache)
	, mSccType(sccType)
	, mSize(0)
	, mNumElements(dims.x * dims.y)
{}

void OmSegmentColorizer::setup()
{
	zi::rwmutex::write_guard g(mMapResizeMutex);

	const OmSegID curSize = mSegmentCache->getMaxValue() + 1;

	if( curSize == mSize ){
		return;
	}

	mSize = curSize;
	mColorCache.resize(curSize);
}

boost::shared_ptr<OmColorRGBA>
OmSegmentColorizer::ColorTile(uint32_t const* imageData)
{
	setup();

	zi::rwmutex::read_guard g(mMapResizeMutex); //prevent vectors from being resized while we're reading

	mAreThereAnySegmentsSelected =
		mSegmentCache->AreSegmentsSelected() ||
		mSegmentCache->AreSegmentsEnabled();

	mCurSegCacheFreshness = OmCacheManager::GetFreshness();

	boost::shared_ptr<OmColorRGBA> colorMappedDataPtr
		= OmSmartPtr<OmColorRGBA>::MallocNumElements(mNumElements,
													 om::DONT_ZERO_FILL);

	doColorTile(imageData, colorMappedDataPtr.get());

	return colorMappedDataPtr;
}

void OmSegmentColorizer::doColorTile(uint32_t const*const d,
									 OmColorRGBA* colorMappedData)
{
	OmColor prevColor = blackColor;
	OmSegID lastVal = 0;

	for(uint32_t i = 0; i < mNumElements; ++i ) {

		colorMappedData[i].alpha = 255;

		if( d[i] == lastVal ){ // memoized previous, non-zero color
			colorMappedData[i].red   = prevColor.red;
			colorMappedData[i].green = prevColor.green;
			colorMappedData[i].blue  = prevColor.blue;

		} else if( 0 == d[i] ){ // black
			colorMappedData[i].red   = 0;
			colorMappedData[i].green = 0;
			colorMappedData[i].blue  = 0;

		} else { // get color from cache

			{ // update cache, if needed
				mutex_guard_t g( d[i] );

				if(mCurSegCacheFreshness > mColorCache[d[i]].freshness){
					mColorCache[d[i]].color = getVoxelColorForView2d(d[i]);
					mColorCache[d[i]].freshness = mCurSegCacheFreshness;
				}
			}

			// memoize
			prevColor = mColorCache[d[i]].color;
			lastVal = d[i];

			colorMappedData[i].red   = prevColor.red;
			colorMappedData[i].green = prevColor.green;
			colorMappedData[i].blue  = prevColor.blue;
		}
	}
}

OmColor OmSegmentColorizer::getVoxelColorForView2d(const OmSegID val)
{
	OmSegment* seg = mSegmentCache->GetSegment(val);
	if( NULL == seg ) {
		return blackColor;
	}
	OmSegment* segRoot = mSegmentCache->findRoot(seg);
	const OmColor segRootColor = segRoot->GetColorInt();

	const bool isSelected =
		mSegmentCache->IsSegmentSelected(segRoot) ||
		mSegmentCache->isSegmentEnabled(segRoot->value());

	switch(mSccType){
	case SCC_SEGMENTATION_VALID:
	case SCC_FILTER_VALID:
		if(seg->IsValidListType()) {
			return segRootColor;
		}
		return blackColor;
	case SCC_SEGMENTATION_VALID_BLACK:
	case SCC_FILTER_VALID_BLACK:
		if(seg->IsValidListType()) {
			return blackColor;
		}
		return segRootColor;
	case SCC_FILTER_BREAK:
	case SCC_SEGMENTATION_BREAK:
		if(isSelected){
			return seg->GetColorInt();;
		}
		return blackColor;
	default:
		if(isSelected){
			return makeSelectedColor(segRootColor);
		}
		if(SCC_FILTER_BLACK == mSccType && mAreThereAnySegmentsSelected){
			return blackColor;
		}
		return segRootColor;
	}
}
