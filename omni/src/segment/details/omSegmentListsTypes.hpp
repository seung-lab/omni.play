#ifndef OM_SEGMENT_LISTS_TYPES_HPP
#define OM_SEGMENT_LISTS_TYPES_HPP

#include "common/omCommon.h"

class OmSegIDsListWithPage {
 public:
	OmSegIDsListWithPage(boost::shared_ptr<OmSegIDsList> list,
						 const int pageOffset )
		: list_(list)
		, mPageOffset(pageOffset)
	{}

	boost::shared_ptr<OmSegIDsList> list_;
	int mPageOffset;
};

#endif
