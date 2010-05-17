#include "omSegmentIterator.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"

OmSegmentIterator::OmSegmentIterator( OmSegmentCache * cache )
{
	mCache = cache;
}

void OmSegmentIterator::iterOverSelectedIDs()
{
	foreach( OmId segID, mCache->GetSelectedSegmentIdsRef() ){
		//printf("adding selected %d\n", segID );
		mSegs.append( mCache->GetSegmentFromID( segID ));
	}
}

void OmSegmentIterator::iterOverEnabledIDs()
{
	foreach( OmId segID, mCache->GetEnabledSegmentIdsRef() ){
		//printf("adding enabled %d\n", segID );
		mSegs.append( mCache->GetSegmentFromID( segID) );
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

	OmSegment * segRet = mSegs.takeLast();
	for( int i = 0; i < segRet->segmentsJoinedIntoMe.size(); ++i){
		mSegs.append( mCache->GetSegmentFromID( segRet->segmentsJoinedIntoMe.at(i) ) );
	}

	return segRet;
}

OmSegmentIterator & OmSegmentIterator::operator = (const OmSegmentIterator & other)
{
	if (this == &other) {
		return *this; 
	}
	
	mCache = other.mCache;

	for( int i = 0; i < other.mSegs.size(); ++i ){
		mSegs.append( other.mSegs.at(i) );
	}

	return *this;
}
