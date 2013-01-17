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
	std::map<std::string, server::tile>  ret;
	const volume::volume channel(URI, common::CHANNEL);
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const coords::globalBbox Bbox(coords::global(1), coords::global(128));

	handler::get_tiles(ret, channel, Bbox, common::XY_VIEW, 0);
	handler::get_tiles(ret, channel, Bbox, common::XZ_VIEW, 1);
	handler::get_tiles(ret, channel, Bbox, common::ZY_VIEW, 2);

	handler::get_tiles(ret, segmentation, Bbox, common::XY_VIEW, 0);
	handler::get_tiles(ret, segmentation, Bbox, common::XZ_VIEW, 1);
	handler::get_tiles(ret, segmentation, Bbox, common::ZY_VIEW, 2);
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