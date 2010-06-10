#include "omSegmentIterator.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"

OmSegmentIterator::OmSegmentIterator( OmSegmentCache * cache, 
				      const bool in_iterOverSelectedIDs, 
				      const bool in_iterOverEnabledIDs )
{
	mCache = cache;
	if( in_iterOverSelectedIDs ){
		iterOverSelectedIDs();
	} 

	if( in_iterOverEnabledIDs ){
		iterOverEnabledIDs();
	}
}

void OmSegmentIterator::iterOverSelectedIDs()
{
	const OmSegIDsSet & set = mCache->GetSelectedSegmentIdsRef();
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){ 
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ));
	}
}

void OmSegmentIterator::iterOverEnabledIDs()
{
	const OmSegIDsSet & set = mCache->GetEnabledSegmentIdsRef();
	OmSegIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){ 
		mSegs.push_back( mCache->GetSegmentFromValue( *iter ) );
	}
}

void OmSegmentIterator::iterOverSegmentIDs(const OmSegIDsSet & set)
{
        OmSegIDsSet::const_iterator iter;
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

	const OmSegIDsSet & set = segRet->segmentsJoinedIntoMe;
	OmSegIDsSet::const_iterator iter;
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
	mSegs = other.mSegs;

	return *this;
}
