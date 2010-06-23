#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCacheImpl.h"

OmSegmentIteratorLowLevel::OmSegmentIteratorLowLevel( OmSegmentCacheImpl * cache )
	: mCache(cache)
{
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
