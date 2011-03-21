#ifndef SEGMENT_TESTS_HPP
#define SEGMENT_TESTS_HPP

#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/colorizer/omSegmentColorizer.h"
#include "tests/fakeMemMapFile.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegmentListBySizeTests.hpp"

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
