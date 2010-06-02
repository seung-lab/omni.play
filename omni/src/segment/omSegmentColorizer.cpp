#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"

#include <QMutexLocker>

static const OmColor blackColor = {0, 0, 0};

OmSegmentColorizer::OmSegmentColorizer( OmSegmentCache * cache, const OmSegmentColorCacheType sccType)
	: mSegmentCache(cache)
	, mSccType(sccType)
	, mColorCache( NULL )
	, mColorCacheFreshness( NULL )
	, mSize( 0 )
	, mCurBreakThreshhold(0)
	, mPrevBreakThreshhold(0)
{
}

void OmSegmentColorizer::setup()
{
	const quint32 curSize = mSegmentCache->getMaxValue() + 1;

	if( mSize > 0 && curSize != mSize ){
		delete [] mColorCache;
		mColorCache = NULL;
		delete [] mColorCacheFreshness;
		mColorCacheFreshness = NULL;
	}

	if( NULL != mColorCache ){
		return;
	}

	mSize = curSize;

	mColorCache = new OmColor[ mSize ];

	mColorCacheFreshness = new int[ mSize ];
	memset(mColorCacheFreshness, 0, sizeof(int) * mSize);
}

void OmSegmentColorizer::colorTile( OmSegID * imageData, const int size,
				    unsigned char * data )
{
	QMutexLocker lock( &mMutex );
	
	setup();

	mSegmentCache->mMutex.lock();
	const int segCacheFreshness = mSegmentCache->mImpl->mCachedColorFreshness;
	mSegmentCache->mMutex.unlock();

	const bool isSegmentation = (SCC_SEGMENTATION == mSccType || 
				     SCC_SEGMENTATION_BREAK == mSccType || 
				     SCC_SEGMENTATION_VALID == mSccType);
	bool showOnlySelectedSegments = mSegmentCache->AreSegmentsSelected();
	if ( isSegmentation ) {
		showOnlySelectedSegments = false;	
	}

	int offset = 0;
	OmColor newcolor = {0, 0, 0};
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
				if( !isCacheElementValid(val, segCacheFreshness) ){
					mColorCache[ val ] = getVoxelColorForView2d( val, showOnlySelectedSegments );
					mColorCacheFreshness[ val ] = segCacheFreshness;
				}
				
				newcolor = mColorCache[ val ];
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
	mSegmentCache->mMutex.lock(); // LOCK (6 unlock possibilities)

	OmSegment * seg = mSegmentCache->mImpl->GetSegmentFromValue( val );
	if( NULL == seg ) {
		mSegmentCache->mMutex.unlock(); //UNLOCK possibility #1 of 6
		return blackColor;
	}
	OmSegment * segRoot = mSegmentCache->mImpl->findRoot( seg );
	const bool isSelected = mSegmentCache->mImpl->isSegmentSelected(segRoot);

	if( SCC_SEGMENTATION_VALID == mSccType || SCC_FILTER_VALID == mSccType){
		if(seg->GetImmutable()) {
			mSegmentCache->mMutex.unlock(); //UNLOCK possibility #2 of 6
			return segRoot->mColorInt;
		} else {
			mSegmentCache->mMutex.unlock(); //UNLOCK possibility #3 of 6
			return blackColor;
		}
	}

	if( SCC_SEGMENTATION_BREAK == mSccType){
		if( isSelected ){
			// const OmColor & tsc = mSegmentCache->mImpl->GetColorAtThreshold( seg, mCurBreakThreshhold );
			const OmColor & tsc = seg->mColorInt;
			mSegmentCache->mMutex.unlock(); //UNLOCK possibility #4 of 6
			return tsc;
		} else {
			mSegmentCache->mMutex.unlock(); //UNLOCK possibility #5 of 6
			return blackColor;
		}
	}

	mSegmentCache->mMutex.unlock(); //UNLOCK possibility #6 of 6

	const OmColor & sc = segRoot->mColorInt;

	if( isSelected ){
		OmColor color = { makeSelectedColor(sc.red),
				  makeSelectedColor(sc.green),
				  makeSelectedColor(sc.blue) };
		return color;
	} else {
		if (showOnlySelectedSegments) {
			return blackColor;
		} else {
			return sc;
		}
	}
}

