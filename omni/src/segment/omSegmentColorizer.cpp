#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/cache/omCacheManager.h"

static const OmColor blackColor = {0, 0, 0};

OmSegmentColorizer::OmSegmentColorizer( boost::shared_ptr<OmSegmentCache> cache,
					const OmSegmentColorCacheType sccType,
					const bool isSegmentation)
	: mSegmentCache(cache)
	, mSccType(sccType)
	, mSize(0)
	, mCurBreakThreshhold(0)
	, mPrevBreakThreshhold(0)
	, mIsSegmentation(isSegmentation)
{
}

OmSegmentColorizer::~OmSegmentColorizer()
{
}

void OmSegmentColorizer::setup()
{
	zi::WriteGuard g(mMapResizeMutex);

	const quint32 curSize = mSegmentCache->getMaxValue() + 1;

	if( curSize == mSize ){
		return;
	}

	mSize = curSize;
	mColorCache.resize(curSize);
	mColorUpdateMutex.resize(curSize);

	printf("segment color cache (%p): current size in memory ~%lu bytes for %d elements\n",
	       this,
	       sizeof(OmColorWithFreshness)*curSize +
	       sizeof(zi::Mutex)*curSize,
	       curSize);
}

void OmSegmentColorizer::colorTile(boost::shared_ptr<uint32_t> imageDataPtr,
				   const int size,
				   boost::shared_ptr<uint8_t> dataPtr )
{
	setup();

	mAreThereAnySegmentsSelected = mSegmentCache->AreSegmentsSelected() ||
		                       mSegmentCache->AreSegmentsEnabled();

	const int segCacheFreshness = OmCacheManager::Freshen(false);
	uint32_t offset = 0;
	OmColor prevColor = blackColor;
	OmSegID lastVal = 0;

	zi::ReadGuard g(mMapResizeMutex); //prevent vectors from being resized while we're reading

	uint32_t* imageData = imageDataPtr.get();
	uint8_t* data = dataPtr.get();

	// looping through each segID in image data (typically 128*128 elements)
	for (int i = 0; i < size; ++i ) {

		const OmSegID val = imageData[i]; // may upcast
		OmColor curColor;

		if(val == lastVal){ // memoized previous, non-zero color
			curColor = prevColor;
		} else if(0 == val){
			curColor = blackColor;
		} else { // get color from cache

			mColorUpdateMutex[val].lock();

			// check if cache element is valid
			if(segCacheFreshness   == mColorCache[val].freshness &&
			   mCurBreakThreshhold == mPrevBreakThreshhold       ){

				curColor = mColorCache[val].color;

			} else { // update color
				curColor = mColorCache[val].color
					 = getVoxelColorForView2d(val);
				mColorCache[val].freshness = segCacheFreshness;
			}

			mColorUpdateMutex[val].unlock();

			prevColor = curColor; // memoize previous, non-zero color
			lastVal   = val;
		}

		data[offset]     = curColor.red;
		data[offset + 1] = curColor.green;
		data[offset + 2] = curColor.blue;
		data[offset + 3] = 255;

		offset += 4;
	}
}

OmColor OmSegmentColorizer::getVoxelColorForView2d( const OmSegID val)
{
	QMutexLocker locker(&mSegmentCache->mMutex);

	OmSegment * seg = mSegmentCache->mImpl->GetSegmentFromValue( val );
	if( NULL == seg ) {
		return blackColor;
	}
	OmSegment * segRoot = mSegmentCache->mImpl->findRoot( seg );
	const OmColor segRootColor = segRoot->GetColorInt();

	const bool isSelected =
		mSegmentCache->mImpl->isSegmentSelected(segRoot) ||
		mSegmentCache->mImpl->isSegmentEnabled(segRoot->value);

	locker.unlock(); // done w/ lock

	if( SCC_SEGMENTATION_VALID == mSccType || SCC_FILTER_VALID == mSccType){
		if(seg->GetImmutable()) {
			return segRootColor;
		} else {
			return blackColor;
		}
	}
	if( SCC_SEGMENTATION_VALID_BLACK == mSccType || SCC_FILTER_VALID_BLACK == mSccType){
		if(seg->GetImmutable()) {
			return blackColor;
		} else {
			return segRootColor;
		}
	}

	if( SCC_FILTER_BREAK == mSccType || SCC_SEGMENTATION_BREAK == mSccType){
		if( isSelected ){
			return seg->GetColorInt();;
		} else {
			return blackColor;
		}
	}

	if( isSelected ){
		OmColor color = { makeSelectedColor(segRootColor.red),
				  makeSelectedColor(segRootColor.green),
				  makeSelectedColor(segRootColor.blue) };
		return color;
	} else {
		if(SCC_FILTER_BLACK == mSccType && mAreThereAnySegmentsSelected){
			return blackColor;
		} else {
			return segRootColor;
		}
	}
}
