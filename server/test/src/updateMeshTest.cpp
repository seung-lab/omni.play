#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "RealTimeMesher.h"

#include "volume/volume.h"
#include "serverHandler.hpp"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

class MockRealTimeMesher : public RealTimeMesherIf
{
public:
	MOCK_METHOD3(updateChunk, bool(const std::string& uri,
				const zi::mesh::Vector3i& chunk,
				const std::string& data));
	MOCK_METHOD4(update, bool(const std::string& uri,
				const zi::mesh::Vector3i& location,
				const zi::mesh::Vector3i& size,
				const std::string& data));
	MOCK_METHOD5(maskedUpdate, bool(const std::string& uri,
				const zi::mesh::Vector3i& location,
				const zi::mesh::Vector3i& size,
				const std::string& data,
				const std::string& mask));
	MOCK_METHOD3(getMesh, void(MeshDataResult& _return,
				const std::string& uri,
				const MeshCoordinate& coordinate));
	MOCK_METHOD3(getMeshes, void(std::vector<MeshDataResult> & _return,
				const std::string& uri,
				const std::vector<MeshCoordinate> & coordinates));
	MOCK_METHOD4(getMeshIfNewer, void(MeshDataResult& _return,
				const std::string& uri,
				const MeshCoordinate& coordinate,
				const int64_t version));
	MOCK_METHOD4(getMeshesIfNewer, void(std::vector<MeshDataResult> & _return,
				const std::string& uri,
				const std::vector<MeshCoordinate> & coordinates,
				const std::vector<int64_t> & versions));
	MOCK_METHOD2(getMeshVersion, int64_t(const std::string& uri,
				const MeshCoordinate& coordinate));
	MOCK_METHOD3(getMeshVersions, void(std::vector<int64_t> & _return,
				const std::string& uri,
				const std::vector<MeshCoordinate> & coordinates));
};

TEST(UpdateMeshTest, Update)
{
	MockRealTimeMesher mesher;

	EXPECT_CALL(mesher, update(_,_,_,_));

	om::volume::volume vol("test/data/test.omni.files/",
		coords::globalBbox(coords::global(0, 0, 0), coords::global(255, 255, 159)),
		vmml::Vector3i::ONE, server::dataType::UINT32,
		server::volType::SEGMENTATION, vmml::Vector3i(128),0);

	std::set<uint32_t> segIds;
	segIds.insert(342);
	segIds.insert(3463);
	segIds.insert(4368);
	segIds.insert(13);

	uint32_t segId = 1;

	handler::update_global_mesh(&mesher, vol, segIds, segId);
}

TEST(UpdateMeshTest, MaskedUpdate)
{
	MockRealTimeMesher mesher;

	EXPECT_CALL(mesher, maskedUpdate(_,_,_,_,_));

	om::volume::volume vol("test/data/test.omni.files/",
		coords::globalBbox(coords::global(0, 0, 0), coords::global(255, 255, 159)),
		vmml::Vector3i::ONE, server::dataType::UINT32,
		server::volType::SEGMENTATION, vmml::Vector3i(128),0);

	std::set<uint32_t> added;
	added.insert(238);

	std::set<uint32_t> modified;
	modified.insert(added.begin(), added.end());

	uint32_t segId = 37;

	handler::modify_global_mesh_data(&mesher, vol, added, modified, segId);
}

// TEST(ThriftTest, UpdateTest)
// {
// 	server::serverHandler handler("18.4.45.150", 9099);
// 	boost::shared_ptr<zi::mesh::RealTimeMesherClient> client = handler.makeMesher();
// 	zi::mesh::RealTimeMesherIf* mesher = client.get();
// 	zi::mesh::Vector3i loc,size;
// 	size.x = size.y = size.z = 1;
// 	loc.x = loc.y = loc.z = 1;
// 	mesher->update("61", loc, size, "1atartfrtfrftrtt");
// }

}} // namespace om::test::
