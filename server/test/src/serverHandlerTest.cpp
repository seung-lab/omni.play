#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "volume/volume.h"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

const char* URI = "test/data/test.omni.files/";
TEST(ServerHandlerTest, get_chunk)
{
	std::string ret;
	const volume::volume channel(URI, common::CHANNEL);
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const server::vector3i chunk;

	handler::get_chunk(ret, channel, chunk);
	handler::get_chunk(ret, segmentation, chunk);
}

// Unimplemented
// TEST(ServerHandlerTest, get_mst)
// {
// 	std::vector<server::edge> ret;
// 	const volume::volume segmentation(URI, common::SEGMENTATION);

// 	handler::get_mst(ret, segmentation);
// }

// Unimplemented
// TEST(ServerHandlerTest, get_graph)
// {
// 	std::vector<server::edge> ret;
// 	const volume::volume segmentation(URI, common::SEGMENTATION);

// 	handler::get_graph(ret, segmentation);
// }

TEST(ServerHandlerTest, get_chan_tile)
{
	server::tile ret;
	const volume::volume channel(URI, common::CHANNEL);
	const coords::global point(1);

	handler::get_chan_tile(ret, channel, point, common::XY_VIEW);
	handler::get_chan_tile(ret, channel, point, common::XZ_VIEW);
	handler::get_chan_tile(ret, channel, point, common::ZY_VIEW);
}

TEST(ServerHandlerTest, get_seg_tiles)
{
	std::map<std::string, server::tile>  ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const int32_t segId = 10;
	const coords::globalBbox segBbox(coords::global(1), coords::global(128));

	handler::get_seg_tiles(ret, segmentation, segId, segBbox, common::XY_VIEW);
	handler::get_seg_tiles(ret, segmentation, segId, segBbox, common::XZ_VIEW);
	handler::get_seg_tiles(ret, segmentation, segId, segBbox, common::ZY_VIEW);
}

TEST(ServerHandlerTest, get_seg_id)
{
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const coords::global point(1);

	int32_t segId = handler::get_seg_id(segmentation, point);
}

TEST(ServerHandlerTest, get_seg_ids)
{
	std::set<int32_t>  ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const coords::global point(1);
	const int32_t radius = 5;

	handler::get_seg_ids(ret, segmentation, point, radius, common::XY_VIEW);
	handler::get_seg_ids(ret, segmentation, point, radius, common::XZ_VIEW);
	handler::get_seg_ids(ret, segmentation, point, radius, common::ZY_VIEW);
}

TEST(ServerHandlerTest, get_seg_data)
{
	server::segData ret;
	const volume::volume segmentation(URI, common::SEGMENTATION);
	const int32_t segId = 10;

	handler::get_seg_data(ret, segmentation, segId);
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

TEST(ServerHandlerTest, compare_results)
{
	const std::vector<server::result> old_results;
	const server::result new_result;

	double comparison = handler::compare_results(old_results, new_result);
}

TEST(ServerHandlerTest, get_mesh)
{
	std::string ret;
	const std::string uri = std::string(URI) + "segmentations/segmentation1/meshes/1.000/";
	const server::vector3i chunk;
	int32_t segId = 15;

	handler::get_mesh(ret, uri, chunk, 0, segId);
	handler::get_mesh(ret, uri, chunk, 1, segId);
}

TEST(ServerHandlerTest, get_obj)
{
	std::string ret;
	const std::string uri = std::string(URI) + "segmentations/segmentation1/meshes/1.000/";
	const server::vector3i chunk;
	int32_t segId = 15;

	handler::get_obj(ret, uri, chunk, 0, segId);
	handler::get_obj(ret, uri, chunk, 1, segId);
}

}} // namespace om::test::