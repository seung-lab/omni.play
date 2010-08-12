#include "omSegmentIterator.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"

OmSegmentIterator::OmSegmentIterator(boost::shared_ptr<OmSegmentCache> cache)
	: mCache(cache)
{
}

void OmSegmentIterator::iterOverSegmentID(const OmSegID segID)
{
	mSegs.push_back(mCache->GetSegment(segID));
}

void OmSegmentIterator::iterOverSelectedIDs()
{
	const OmSegIDsSet & set = mCache->GetSelectedSegmentIds();
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( mCache->GetSegment( *iter ));
	}
}

void OmSegmentIterator::iterOverEnabledIDs()
{
	const OmSegIDsSet & set = mCache->GetEnabledSegmentIds();
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( mCache->GetSegment( *iter ) );
	}
}

void OmSegmentIterator::iterOverSegmentIDs(const OmSegIDsSet & set)
{
        OmSegIDsSet::const_iterator iter;
        for( iter = set.begin(); iter != set.end(); ++iter ){
                mSegs.push_back( mCache->GetSegment( *iter ) );
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

	const OmSegIDsSet & set = segRet->segmentsJoinedIntoMe;
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		mSegs.push_back( mCache->GetSegment( *iter ));
	}

	return segRet;
}

OmSegmentIterator & OmSegmentIterator::operator = (const OmSegmentIterator & other)
{
	if (this == &other) {
		return *this;
	}

	mCache = other.mCache;
	mSegs = other.mSegs;

	return *this;
}
