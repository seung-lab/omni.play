#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"

#include <QMutexLocker>

OmSegmentColorizer::OmSegmentColorizer( OmSegmentCache * cache, const OmSegmentColorCacheType sccType)
	: mSegmentCache(cache), 
	  mSccType(sccType),
	  mColorCache( NULL ),
	  mColorCacheFreshness( NULL ),
	  mSize( 0 )
{
}

void OmSegmentColorizer::setup()
{
	mSize = mSegmentCache->mImpl->mMaxValue + 1;
	mColorCache = new OmColor[ mSize ];
	mColorCacheFreshness = std::vector<int>( mSize, 0);
}

void OmSegmentColorizer::colorTile( SEGMENT_DATA_TYPE * imageData, const int size,
				    unsigned char * data )
{
	if( NULL == mColorCache ){
		setup();
	}

	const bool isSegmentation = (Segmentation == mSccType || SegmentationBreak == mSccType);

	const int segCacheFreshness = mSegmentCache->mImpl->mCachedColorFreshness;

	bool showOnlySelectedSegments = mSegmentCache->AreSegmentsSelected();
	if ( isSegmentation ) {
		showOnlySelectedSegments = false;	
	}

	int ctr = 0;
	OmColor newcolor = {0, 0, 0};
	SEGMENT_DATA_TYPE lastVal = 0;
	SEGMENT_DATA_TYPE val;
	
	// looping through each value of imageData, which is 
	//   strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0; i < size; ++i ) {

		val = (SEGMENT_DATA_TYPE) imageData[i];

		if ( val != lastVal) {
			if( segCacheFreshness != mColorCacheFreshness[ val ] ){
				mColorCache[ val ] = getVoxelColorForView2d( val, showOnlySelectedSegments );
				mColorCacheFreshness[ val ] = segCacheFreshness;
			} 
			
			newcolor = mColorCache[ val ];
		} 

		data[ctr]     = newcolor.red;
		data[ctr + 1] = newcolor.green;
		data[ctr + 2] = newcolor.blue;
		data[ctr + 3] = 255;

		ctr = ctr + 4;
		lastVal = val;
	}

}

OmColor OmSegmentColorizer::getVoxelColorForView2d( const SEGMENT_DATA_TYPE val, 
						    const bool showOnlySelectedSegments)
{
	QMutexLocker lock( &mSegmentCache->mMutex );

	OmColor color = {0,0,0};

	OmSegment * seg = mSegmentCache->mImpl->GetSegmentFromValue( val );
	if( NULL == seg ) {
		return color;
	}

	OmSegment * segRoot = mSegmentCache->mImpl->findRoot( seg );

	if(SegmentationBreak == mSccType){
		if( mSegmentCache->mImpl->isSegmentSelected(segRoot)) {
                	color.red   = seg->mColor.x * 255;
                	color.green = seg->mColor.y * 255;
                	color.blue  = seg->mColor.z * 255;
			return color;
		} 
	}

	const Vector3 < float > & sc = segRoot->mColor;

	if( mSegmentCache->mImpl->isSegmentSelected(segRoot)) {
		color.red   = clamp(sc.x * 255 * 2.5);
		color.green = clamp(sc.y * 255 * 2.5);
		color.blue  = clamp(sc.z * 255 * 2.5);
	} else {
		if (showOnlySelectedSegments) {
			// don't show
		} else {
			color.red   = sc.x * 255;
			color.green = sc.y * 255;
			color.blue  = sc.z * 255;
		}
	}

	return color;
}
