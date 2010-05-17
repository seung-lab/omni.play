#ifndef OM_SEGMENT_ITERATOR_H
#define OM_SEGMENT_ITERATOR_H

#include "common/omCommon.h"
#include <QList>

class OmSegment;
class OmSegmentCache;

class OmSegmentIterator
{
 public:
	OmSegmentIterator( OmSegmentCache * cache );
	void iterOverSelectedIDs();
	void iterOverEnabledIDs();

	OmSegment * getNextSegment();
	bool empty();
	
	OmSegmentIterator & operator = (const OmSegmentIterator & other);

 private:
	OmSegmentCache * mCache;

	QList< OmSegment* > mSegs;
};

#endif
