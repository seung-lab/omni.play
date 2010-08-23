#include <zi/utility>
#include "segment/omSegmentIterator.h"
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
	FOR_EACH(iter, mCache->GetSelectedSegmentIds()){
		mSegs.push_back( mCache->GetSegment( *iter ));
	}
}

void OmSegmentIterator::iterOverEnabledIDs()
{
	FOR_EACH(iter, mCache->GetEnabledSegmentIds()){
		mSegs.push_back( mCache->GetSegment( *iter ) );
	}
}

void OmSegmentIterator::iterOverSegmentIDs(const OmSegIDsSet & set)
{
	FOR_EACH(iter, set){
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

	FOR_EACH(iter, segRet->getChildren() ){
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
