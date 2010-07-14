#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "system/omCacheManager.h"

#include <QMutexLocker>

static const OmColor blackColor = {0, 0, 0};

OmSegmentColorizer::OmSegmentColorizer( OmSegmentCache * cache, 
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
	QWriteLocker mapResizeLock(&mMapResizeMutex);

	const quint32 curSize = mSegmentCache->getMaxValue() + 1;

	if( curSize == mSize ){
		return;
	}

	mSize = curSize;
	mColorCache.resize(curSize);
}

void OmSegmentColorizer::colorTile( OmSegID * imageData, const int size,
				    unsigned char * data )
{
	setup();

	QReadLocker mapResizeLock(&mMapResizeMutex);

	const int segCacheFreshness = OmCacheManager::Freshen(false);

	bool showOnlySelectedSegments = mSegmentCache->AreSegmentsSelected();
	if(mIsSegmentation) {
		showOnlySelectedSegments = false;	
	}

	int offset = 0;
	OmColor newcolor = blackColor;
	OmSegID lastVal = 0;
	OmSegID val;

	// looping through each value of imageData, which is 
	//   strictly dims.x * dims.y big, no extra because of cast to OmSegID
	for (int i = 0; i < size; ++i ) {

		val = (OmSegID) imageData[i];

		if ( val != lastVal) {
			if( 0 == val ){
				newcolor = blackColor;
			} else{
				mColorUpdateMutex.lock(); // TODO: use lock-free hash?
				if( !isCacheElementValid(val, segCacheFreshness) ){
					mColorCache[ val ].color = getVoxelColorForView2d( val, showOnlySelectedSegments );
					mColorCache[ val ].freshness = segCacheFreshness;
				}
				mColorUpdateMutex.unlock();

				newcolor = mColorCache[ val ].color;
			}
		} 

		data[offset]     = newcolor.red;
		data[offset + 1] = newcolor.green;
		data[offset + 2] = newcolor.blue;
		data[offset + 3] = 255;

		offset += 4;
		lastVal = val;
	}
}

OmColor OmSegmentColorizer::getVoxelColorForView2d( const OmSegID val, 
						    const bool showOnlySelectedSegments)
{
	QMutexLocker locker(&mSegmentCache->mMutex);

	OmSegment * seg = mSegmentCache->mImpl->GetSegmentFromValue( val );
	if( NULL == seg ) {
		return blackColor;
	}
	OmSegment * segRoot = mSegmentCache->mImpl->findRoot( seg );
	const OmColor segRootColor = segRoot->mColorInt;

	const bool isSelected = mSegmentCache->mImpl->isSegmentSelected(segRoot);

	if( SCC_SEGMENTATION_VALID == mSccType || SCC_FILTER_VALID == mSccType){
		if(seg->mImmutable) {
			return segRootColor;
		} else {
			return blackColor;
		}
	}
	if( SCC_SEGMENTATION_VALID_BLACK == mSccType || SCC_FILTER_VALID_BLACK == mSccType){
		if(seg->mImmutable) {
			return blackColor;
		} else {
			return segRootColor;
		}
	}

	if( SCC_FILTER_BREAK == mSccType || SCC_SEGMENTATION_BREAK == mSccType){
		if( isSelected ){
			return seg->mColorInt;;
		}
	}

	locker.unlock(); // done w/ lock

	if( isSelected ){
		OmColor color = { makeSelectedColor(segRootColor.red),
				  makeSelectedColor(segRootColor.green),
				  makeSelectedColor(segRootColor.blue) };
		return color;
	} else {
		if (showOnlySelectedSegments) {
			return blackColor;
		} else {
			return segRootColor;
		}
	}
}
