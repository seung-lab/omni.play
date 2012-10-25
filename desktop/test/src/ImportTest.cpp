#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "volume/build/omBuildChannel.hpp"
#include "volume/build/omBuildSegmentation.hpp"

namespace om {
namespace test {

TEST(ImportTest, ImportChan)
{
	OmBuildChannel bc;

    bc.addFileNameAndPath("test/data/test_chan.h5");
	bc.Build();
}

TEST(ImportTest, ImportSeg)
{
	OmBuildSegmentation bs;

    bs.addFileNameAndPath("test/data/test_seg.h5");
	bs.BuildImage();
}

}} // namespace om::test::