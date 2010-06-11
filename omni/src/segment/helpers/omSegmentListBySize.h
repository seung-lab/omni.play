#ifndef OM_SEGMENT_LIST_BY_SIZE_H
#define OM_SEGMENT_LIST_BY_SIZE_H

#include "common/omCommon.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using namespace boost::multi_index;

class OmSegmentListBySize
{

 public:
	OmSegmentListBySize(){}

 private:

	class OmSegSize
	{
	 public:
		OmSegSize( const OmSegID segID_, const quint64 segSize_ ) 
			: segID(segID_), segSize(segSize_){}
		
		OmSegID segID;
		quint64 segSize;
	};

	struct segID{};
	struct segSize{};

	typedef boost::multi_index_container
	< OmSegSize,
	  indexed_by
	  < ordered_unique<     tag<segID>,   BOOST_MULTI_INDEX_MEMBER(OmSegSize,OmSegID,segID)>,
	    ordered_non_unique< tag<segSize>, BOOST_MULTI_INDEX_MEMBER(OmSegSize,quint64,segSize), std::greater<quint64> > 
	  >
	> OmSegSizes;

	OmSegSizes mList;
};

#endif
