#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/cache/omCacheManager.h"
#include "utility/omSmartPtr.hpp"

static const OmColor blackColor = {0, 0, 0};

OmSegmentColorizer::OmSegmentColorizer( boost::shared_ptr<OmSegmentCache> cache,
					const OmSegmentColorCacheType sccType,
					const Vector2i& dims)
	: mSegmentCache(cache)
	, mSccType(sccType)
	, mSize(0)
	, mCurBreakThreshhold(0)
	, mPrevBreakThreshhold(0)
	, mNumElements(dims.x * dims.y)
{
}

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
OmSegmentColorizer::colorTile(boost::shared_ptr<uint32_t> imageDataPtr)
{
	setup();

	zi::rwmutex::read_guard g(mMapResizeMutex); //prevent vectors from being resized while we're reading

	mAreThereAnySegmentsSelected = mSegmentCache->AreSegmentsSelected() ||
		                       mSegmentCache->AreSegmentsEnabled();

	mCurSegCacheFreshness = OmCacheManager::GetFreshness();

	boost::shared_ptr<OmColorRGBA> colorMappedDataPtr
		= OmSmartPtr<OmColorRGBA>::makeMallocPtrNumElements(mNumElements);

	doColorTile(imageDataPtr.get(), colorMappedDataPtr.get());

	return colorMappedDataPtr;
}

void OmSegmentColorizer::doColorTile(uint32_t* imageData,
				     OmColorRGBA* colorMappedData)
{
	OmColor prevColor = blackColor;
	OmSegID lastVal = 0;

	for(uint32_t i = 0; i < mNumElements; ++i ) {

		const OmSegID val = imageData[i]; // may upcast
		OmColor curColor;

		if(val == lastVal){ // memoized previous, non-zero color
			curColor = prevColor;
		} else if(0 == val){
			curColor = blackColor;
		} else { // get color from cache

			{
				zi::spinlock::pool<segment_colorizer_mutex_pool_tag>::guard g(val);

				// check if cache element is valid
				if(mCurSegCacheFreshness  == mColorCache[val].freshness &&
				   mCurBreakThreshhold == mPrevBreakThreshhold ){

					curColor = mColorCache[val].color;

				} else { // update color
					curColor = mColorCache[val].color
						= getVoxelColorForView2d(val);
					mColorCache[val].freshness = mCurSegCacheFreshness;
				}
			}

			prevColor = curColor; // memoize previous, non-zero color
			lastVal   = val;
		}

		colorMappedData[i].red   = curColor.red;
		colorMappedData[i].green = curColor.green;
		colorMappedData[i].blue  = curColor.blue;
		colorMappedData[i].alpha = 255;
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
		mSegmentCache->isSegmentEnabled(segRoot->value);

	switch(mSccType){
	case SCC_SEGMENTATION_VALID:
	case SCC_FILTER_VALID:
		if(seg->GetImmutable()) {
			return segRootColor;
		}
		return blackColor;
	case SCC_SEGMENTATION_VALID_BLACK:
	case SCC_FILTER_VALID_BLACK:
		if(seg->GetImmutable()) {
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
			OmColor color = {makeSelectedColor(segRootColor.red),
					 makeSelectedColor(segRootColor.green),
					 makeSelectedColor(segRootColor.blue)};
			return color;
		}
		if(SCC_FILTER_BLACK == mSccType && mAreThereAnySegmentsSelected){
			return blackColor;
		}
		return segRootColor;
	}
}
