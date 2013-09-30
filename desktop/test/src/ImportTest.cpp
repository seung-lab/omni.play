#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "volume/build/omBuildChannel.hpp"
#include "volume/build/omBuildSegmentation.hpp"

namespace om {
namespace test {

// class ImportTest : public ::testing::Test
// {
// protected:
// 	ImportTest()
// 		: bc(ChannelDataWrapper(1))
// 		, bs(SegmentationDataWrapper(1))
// 	{}

// 	OmBuildChannel bc;
// 	OmBuildSegmentation bs;
// };

// TEST_F(ImportTest, ImportChan)
// {
// 	bc.addFileNameAndPath("test/data/test_chan.h5");
// 	bc.Build();
// }

// TEST_F(ImportTest, ImportSeg)
// {
//     bs.addFileNameAndPath("test/data/test_seg.h5");
// 	bs.BuildImage();
// }

// TEST_F(ImportTest, Mesh)
// {
//    bs.BuildMesh();
// }

}
}  // namespace om::test::