#ifndef OM_SEGMENT_LISTS_HPP
#define OM_SEGMENT_LISTS_HPP

#include "segment/lowLevel/omSegmentListBySize.h"

class OmSegmentation;


class OmSegmentLists {
public:
	OmSegmentLists(OmSegmentation* segmentation);

	OmSegmentListBySize mValidListBySize;
	OmSegmentListBySize mRootListBySize;

private:
	OmSegmentation *const segmentation;

};

#endif
