#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "zi/omUtility.h"

OmSegmentIteratorLowLevel::OmSegmentIteratorLowLevel(
	OmSegmentCacheImplLowLevel* cache )
	: mCache(cache)
	, mIterOverAll(false)
	, mCurSegID(0)
{
}

void OmSegmentIteratorLowLevel::iterOverAllSegments()
{
	mIterOverAll = true;
	mSegs.clear();
}

void OmSegmentIteratorLowLevel::iterOverSegmentID(const OmSegID segID)
{
	mIterOverAll = false;
	mSegs.push_back( mCache->GetSegmentFromValue(segID) );
}

bool OmSegmentIteratorLowLevel::empty()
{
	return mSegs.empty();
}

OmSegment* OmSegmentIteratorLowLevel::getNextSegment()
{
	if( mIterOverAll ){
		return getNextSegmentFromFullList();
	} else {
		return getNextSegmentFromSet();
	}
}

OmSegment* OmSegmentIteratorLowLevel::getNextSegmentFromFullList()
{
	OmSegment* seg;
	const OmSegID maxSegValue = mCache->getMaxValue();
	for(OmSegID i = 1+mCurSegID; i <= maxSegValue; ++i){
		seg = mCache->GetSegmentFromValue( i );
		if(!seg){
			continue;
		}
		mCurSegID = i;
		return seg;
	}

	return NULL;
}

OmSegment* OmSegmentIteratorLowLevel::getNextSegmentFromSet()
{
	if( mSegs.empty() ){
		return NULL;
	}

	OmSegment* segRet = mSegs.back();
	mSegs.pop_back();

	FOR_EACH(iter, segRet->getChildren()){
		mSegs.push_back(*iter);
	}

	return segRet;
}
