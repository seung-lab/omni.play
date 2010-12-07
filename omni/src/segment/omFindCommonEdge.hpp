#ifndef OM_FIND_COMMON_EDGE_HPP
#define OM_FIND_COMMON_EDGE_HPP

#include "segment/omSegmentCache.h"

class OmFindCommonEdge {
private:
	static OmSegmentEdge makeEdge()
	{
		return OmSegmentEdge();
	}

	static OmSegmentEdge makeEdge(OmSegment* childSeg)
	{
		return OmSegmentEdge(childSeg->getParent()->value(),
							 childSeg->value(),
							 childSeg->getThreshold());
	}

	static OmSegmentEdge makeEdge(OmSegment* parentSeg, OmSegment* childSeg,
								  const double t)
	{
		return OmSegmentEdge(parentSeg->value(),
							 childSeg->value(),
							 t);
	}

public:
	static OmSegmentEdge FindClosestCommonEdge(OmSegmentCache* segCache,
											   OmSegment* seg1,
											   OmSegment* seg2)
	{
		if( segCache->findRoot(seg1) != segCache->findRoot(seg2) ){
			//debug(dend, "can't split disconnected objects.\n");
			return makeEdge();
		}
		if( seg1 == seg2 ){
			//debug(dend, "can't split object from self.\n");
			return makeEdge();
		}

		OmSegment * s1 = seg1;
		while (0 != s1->getParent()) {
			if(s1->getParent() == seg2) {
				//debug(split, "splitting child from a direct parent\n");
				return makeEdge(s1);
			}
			s1 = s1->getParent();
		}

		OmSegment * s2 = seg2;
		while (0 != s2->getParent()) {
			if(s2->getParent() == seg1) {
				//debug(split, "splitting child from a direct parent\n");
				return makeEdge(s2);
			}
			s2 = s2->getParent();
		}

		OmSegment * nearestCommonPred = 0;

		OmSegment* one;

		for (one = seg1;
			 one != NULL;
			 one = one->getParent())
		{
			OmSegment* two;
			for (two = seg2;
				 two != NULL && one != two;
				 two = two->getParent())
			{
			}

			if (one == two) {
				nearestCommonPred = one;
				break;
			}
		}

		assert(nearestCommonPred != 0);

		double minThresh = 100.0;
		OmSegment * minChild = 0;
		for (one = seg1;
			 one != nearestCommonPred;
			 one = one->getParent())
		{
			if (one->getThreshold() < minThresh) {
				minThresh = one->getThreshold();
				minChild = one;
			}
		}

		for (one = seg2;
			 one != nearestCommonPred;
			 one = one->getParent())
		{
			if (one->getThreshold() < minThresh) {
				minThresh = one->getThreshold();
				minChild = one;
			}
		}

		assert(minChild != 0);
		return makeEdge(minChild);
	}
};

#endif
