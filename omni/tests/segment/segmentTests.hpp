#ifndef SEGMENT_TESTS_HPP
#define SEGMENT_TESTS_HPP

#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentColorizer.h"
#include "tests/fakeMemMapFile.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegmentListBySizeTests.hpp"

#include <boost/make_shared.hpp>

class SegmentTests {
public:
	SegmentTests()
	{}

	void RunAll()
	{
		segmentListBySizeTests();
	}

private:
	void segmentListBySizeTests()
	{
		OmSegmentListBySizeTests segListTests;
		segListTests.RunAll();
	}
};


#endif
