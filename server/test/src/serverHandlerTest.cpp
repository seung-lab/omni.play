#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "volume/volume.h"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

const char* URI = "test/data/test.omni.files/";
TEST(ServerHandlerTest, get_tiles)
{
	std::vector<server::tile>  ret;
	const volume::volume channel(URI, common::CHANNEL);
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const coords::chunk chunk(0, vmml::Vector3i(0));
	const coords::chunk chunk2(1, vmml::Vector3i(0));

	handler::get_tiles(ret, channel, chunk, common::XY_VIEW, 0, 3);
	EXPECT_TRUE(ret.size() > 0);
	handler::get_tiles(ret, channel, chunk2, common::XZ_VIEW, 0, 3);
	EXPECT_TRUE(ret.size() > 0);

	handler::get_tiles(ret, segmentation, chunk, common::XY_VIEW, 0, 3);
	EXPECT_TRUE(ret.size() > 0);
	handler::get_tiles(ret, segmentation, chunk2, common::XZ_VIEW, 0, 3);
	EXPECT_TRUE(ret.size() > 0);
}

TEST(ServerHandlerTest, get_seg_list_data)
{
	std::map<int, server::segData> ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	std::set<int32_t> segIds;
	segIds.insert(5);
	segIds.insert(7);
	segIds.insert(9);
	segIds.insert(11);

	handler::get_seg_list_data(ret, segmentation, segIds);
}

TEST(ServerHandlerTest, get_mesh)
{
	std::string ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const server::vector3i chunk;
	int32_t segId = 122;

	handler::get_mesh(ret, segmentation, chunk, 0, segId);
	EXPECT_TRUE(ret.size() > 0);
	handler::get_mesh(ret, segmentation, chunk, 1, segId);
	EXPECT_TRUE(ret.size() > 0);
}

TEST(ServerHandlerTest, get_obj)
{
	std::string ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const server::vector3i chunk;
	int32_t segId = 122;

	handler::get_obj(ret, segmentation, chunk, 0, segId);
	EXPECT_TRUE(ret.size() > 0);
	handler::get_obj(ret, segmentation, chunk, 1, segId);
	EXPECT_TRUE(ret.size() > 0);
}

}} // namespace om::test::