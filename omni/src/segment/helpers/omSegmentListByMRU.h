#ifndef OM_SEGMENT_LIST_BY_MRU_H
#define OM_SEGMENT_LIST_BY_MRU_H

#include "segment/helpers/omSegmentListBySize.h"

class OmSegmentListByMRU : private OmSegmentListBySize {
 public:
	OmSegmentListByMRU();

	void touch( const OmSegID segID, const quint64 activity );
	OmSegIDsListWithPage * getAPageWorthOfSegmentIDs( const unsigned int, const int, const OmSegID);
};

#endif
