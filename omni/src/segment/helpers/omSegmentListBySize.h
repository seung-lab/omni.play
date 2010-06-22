#ifndef OM_SEGMENT_LIST_BY_SIZE_H
#define OM_SEGMENT_LIST_BY_SIZE_H

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentEdge.h"
#include "segment/omSegmentPointers.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using namespace boost::multi_index;

class OmSegmentListBySize
{

 public:
	OmSegmentListBySize(){}

	void insertSegment( OmSegment * seg );
	void removeSegment( OmSegment * seg );
	void updateFromJoin( OmSegment * root, OmSegment * child );
	void updateFromSplit( OmSegment * root, OmSegment * child );

	static void swapSegment( OmSegment * seg, OmSegmentListBySize * one, OmSegmentListBySize * two );

	OmSegIDsListWithPage * getAPageWorthOfSegmentIDs( const unsigned int, const int, const OmSegID);
	quint64 getSegmentSize( const OmSegID segID_ );

	quint32 size();
	void dump();
	void clear();

 protected:

	void do_incrementSegSize( const OmSegID segID_, const quint64 addedSize );
	void do_removeSegment( const OmSegID segID_ );
	void do_insertSegment( const OmSegID segID_, const quint64 size_ );


	struct OmSegSize {
		OmSegID segID;
		quint64 segSize;
		OmSegSize( const OmSegID segID_, const quint64 segSize_ ) 
			: segID(segID_), segSize(segSize_){}
	};

	struct segID{};
	struct segSize{};

	// TODO: change segID to be hash...
	typedef boost::multi_index_container
	< OmSegSize,
	  indexed_by <
            ordered_non_unique< tag<segSize>, BOOST_MULTI_INDEX_MEMBER(OmSegSize,quint64,segSize), std::greater<quint64> >,
	    ordered_unique<     tag<segID>,   BOOST_MULTI_INDEX_MEMBER(OmSegSize,OmSegID,segID) >
	  >
	> OmSegSizes;

	typedef OmSegSizes::index<segSize>::type List_by_size;
	typedef OmSegSizes::index<segID>::type List_by_ID;

	OmSegSizes mList;

 private:
	void advanceIter(List_by_size & sizeIndex, List_by_size::iterator & iterSize, const int offset);
};

#endif
