#ifndef OM_SEGMENT_BAGS_HPP
#define OM_SEGMENT_BAGS_HPP

#include "segment/omSegmentBag.hpp"

class OmSegmentBags {
private:
	boost::unordered_map<OmSegID, OmSegmentBag> bags_;

public:
	OmSegmentBags()
	{}

};

#endif
