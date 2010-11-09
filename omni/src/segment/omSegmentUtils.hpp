#ifndef OM_SEGMENT_UTILS_HPP
#define OM_SEGMENT_UTILS_HPP

#include "segment/omSegment.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"

class OmSegmentUtils {
public:
	boost::shared_ptr<std::set<OmSegment*> >
	static GetAllChildrenSegments(OmSegmentCache* segCache,
								  const OmSegIDsSet& set)
	{
		OmSegmentIterator iter(segCache);
		iter.iterOverSegmentIDs(set);

		OmSegment * seg = iter.getNextSegment();
		std::set<OmSegment*>* children = new std::set<OmSegment*>();

		while(NULL != seg) {
			children->insert(seg);
			seg = iter.getNextSegment();
		}

		return boost::shared_ptr<std::set<OmSegment*> >(children);
	}

	DataCoord
	static FindCenterOfSelectedSegments(const SegmentationDataWrapper& sdw)
	{
		DataBbox box;

		OmSegmentIterator iter(sdw.GetSegmentCache());
		iter.iterOverSelectedIDs();

		const int max = 5000;

		OmSegment* seg = iter.getNextSegment();
		for(int i = 0; i < max && NULL != seg; ++i){

			const DataBbox& segBox = seg->getBounds();
			if(segBox.isEmpty()){
				continue;
			}

			box.merge(segBox);

			seg = iter.getNextSegment();
		}

		if(box.isEmpty()){
			return DataCoord(0,0,0);
		}

		return (box.getMin() + box.getMax()) / 2;
	}
};

#endif
