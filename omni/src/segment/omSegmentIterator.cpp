#include "omSegmentIterator.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"

OmSegmentIterator::OmSegmentIterator( OmSegmentCache * cache )
{
	mCache = cache;
}

void OmSegmentIterator::iterOverSelectedIDs()
{
	const OmSegIDs & set = mCache->GetSelectedSegmentIdsRef();
	OmSegIDs::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){ 
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ));
	}
}

void OmSegmentIterator::iterOverEnabledIDs()
{
	const OmSegIDs & set = mCache->GetEnabledSegmentIdsRef();
	OmSegIDs::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){ 
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ) );
	}
}

bool OmSegmentIterator::empty()
{
	return mSegs.empty();
}

OmSegment * OmSegmentIterator::getNextSegment()
{
	if( mSegs.empty() ){
		return NULL;
	}

	OmSegment * segRet = mSegs.back();
	mSegs.pop_back();

	const OmSegIDs & set = segRet->segmentsJoinedIntoMe;
	OmSegIDs::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ));
	}

	return segRet;
}

OmSegmentIterator & OmSegmentIterator::operator = (const OmSegmentIterator & other)
{
	if (this == &other) {
		return *this; 
	}
	
	mCache = other.mCache;

	mSegs.reserve( other.mSegs.size() );

	const OmSegPtrs & set = other.mSegs;
	OmSegPtrs::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( *iter );
	}

	return *this;
}
