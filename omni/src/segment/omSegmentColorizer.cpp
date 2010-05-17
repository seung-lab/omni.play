#include "segment/omSegmentColorizer.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"

#include <QMutexLocker>

OmSegmentColorizer::OmSegmentColorizer( OmSegmentCache * cache)
{
	mCache = cache;
}

void OmSegmentColorizer::colorTile( SEGMENT_DATA_TYPE * imageData, const int size,
				    const bool isSegmentation, unsigned char * data )
{
	bool showOnlySelectedSegments = mCache->AreSegmentsSelected();
	if ( isSegmentation ) {
		showOnlySelectedSegments = false;	
	}

	int ctr = 0;
	OmColor newcolor = {0, 0, 0};
	SEGMENT_DATA_TYPE lastVal = 0;
	
	// looping through each value of imageData, which is 
	//   strictly dims.x * dims.y big, no extra because of cast to SEGMENT_DATA_TYPE
	for (int i = 0; i < size; ++i ) {
		SEGMENT_DATA_TYPE curVal = (SEGMENT_DATA_TYPE) imageData[i];

		if (curVal != lastVal) {
			mCache->mMutex.lock();
			newcolor = mCache->mImpl->getVoxelColorForView2d( curVal, showOnlySelectedSegments );
			mCache->mMutex.unlock();
		} 

		data[ctr]     = newcolor.red;
		data[ctr + 1] = newcolor.green;
		data[ctr + 2] = newcolor.blue;
		data[ctr + 3] = 255;

		ctr = ctr + 4;
		lastVal = curVal;
	}

}
