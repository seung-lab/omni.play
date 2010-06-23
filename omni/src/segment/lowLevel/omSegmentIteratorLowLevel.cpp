#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"

OmSegmentIteratorLowLevel::OmSegmentIteratorLowLevel( OmSegmentCacheImplLowLevel * cache )
	: mCache(cache)
	, mIterOverAll(false)
	, mCurSegID(0)
{
}

void OmSegmentIteratorLowLevel::iterOverAllSegments()
{
	mIterOverAll = true;
}

void OmSegmentIteratorLowLevel::iterOverSegmentID(const OmSegID segID )
{
	mSegs.push_back( mCache->GetSegmentFromValue(segID) );
}

bool OmSegmentIteratorLowLevel::empty()
{
	return mSegs.empty();
}

OmSegment * OmSegmentIteratorLowLevel::getNextSegment()
{
	if( mIterOverAll ){
		return getNextSegmentFromFullList();
	} else {
		return getNextSegmentFromSet();
	}
}

OmSegment * OmSegmentIteratorLowLevel::getNextSegmentFromFullList()
{
	OmSegment * seg;
	const OmSegID maxSegValue = mCache->getMaxValue();
	for( OmSegID i = 1+mCurSegID; i <= maxSegValue; ++i ){
		seg = mCache->GetSegmentFromValue( i );
                if( NULL == seg) {
			continue;
		} 
		mCurSegID = i;
		return seg;
	}

	return NULL;
}

OmSegment * OmSegmentIteratorLowLevel::getNextSegmentFromSet()
{
	if( mSegs.empty() ){
		return NULL;
	}

	OmSegment * segRet = mSegs.back();
	mSegs.pop_back();

	const OmSegIDsSet & set = segRet->segmentsJoinedIntoMe;
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ));
	}

	return segRet;
}

OmSegmentIteratorLowLevel & OmSegmentIteratorLowLevel::operator = (const OmSegmentIteratorLowLevel & other)
{
	if (this == &other) {
		return *this; 
	}
	
	mCache = other.mCache;
	mSegs = other.mSegs;

	return *this;
}
