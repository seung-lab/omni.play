#ifndef OM_SEGMENT_LIST_BY_MRU_H
#define OM_SEGMENT_LIST_BY_MRU_H

#include "segment/lowLevel/omSegmentListBySize.h"

class OmSegmentListByMRU : public OmSegmentListBySize {
 public:
	OmSegmentListByMRU();

	void touch( const OmSegID segID );
	OmSegIDsListWithPage * getAPageWorthOfSegmentIDs( const unsigned int, const int, const OmSegID);

 private:
	quint64 getRecentActivity();
};

#endif
