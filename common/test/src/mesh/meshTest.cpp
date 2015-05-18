#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"
#include "volume/segmentation.h"
#include "mesh/vertexIndex/dataSource.h"
#include "chunk/mockUniqueValuesDataSource.hpp"

using ::testing::DefaultValue;
using ::testing::_;

using namespace om::mesh;

namespace om {
namespace test {

#define URI "../../test_data/x06y59z28_s1587_13491_6483_e1842_13746_6738.omni"

class Mesh_VertexIndexMeshes : public ::testing::Test {
 public:
  Mesh_VertexIndexMeshes() {
    auto values = std::make_shared<std::vector<uint32_t>>();
    values->push_back(1);
    values->push_back(2);
    values->push_back(3);
    DefaultValue<std::shared_ptr<std::vector<uint32_t>>>::Set(values);
  }

 protected:
  MockUniqueValuesDataSource uvm;
};

TEST_F(Mesh_VertexIndexMeshes, GetMesh) {
  file::Paths p(URI);
  volume::Segmentation s(p, 1);
  auto& meshes = s.MeshDS();
  std::shared_ptr<VertexIndexMesh> mesh =
      meshes.Get(coords::Mesh(coords::Chunk(0, 0, 0, 0), 1));
  EXPECT_TRUE((bool)mesh);
  EXPECT_TRUE(mesh->Data().VertexIndex());
  EXPECT_TRUE(mesh->Data().StripData());
  EXPECT_TRUE(mesh->Data().VertexData());
}

// TEST_F(Mesh_VertexIndexMeshes, PutMesh)
// {
//  utility::UUID uuid;
//  std::string path = file::tempPath() + "/" + uuid.Str() + "/";
//  file::MkDir(path + "meshes/1.000/0/0/0/0");

//  VertexIndexDataSource meshes(path, &uvm);
//  EXPECT_CALL(uvm, Get(_)).Times(1);
//  coords::Mesh meshCoord(coords::Chunk(0,0,0,0), 1);
//  auto mesg = std::make_shared<VertexIndexMesh>(meshCoord);

//  mesh->vertexIndex().push_back(1);
//  mesh->vertexIndex().push_back(2);
//  mesh->vertexIndex().push_back(3);
//  mesh->vertexIndex().push_back(4);

//  mesh->stripData().push_back(2);
//  mesh->stripData().push_back(4);
//  mesh->stripData().push_back(6);
//  mesh->stripData().push_back(8);

//  mesh->vertexData().push_back(3);
//  mesh->vertexData().push_back(5);
//  mesh->vertexData().push_back(7);
//  mesh->vertexData().push_back(9);

//  meshes.Put(meshCoord, mesh);
//  auto meshBack = meshes.Get(meshCoord);
//  EXPECT_EQ(meshBack->coord(), meshCoord);
//  EXPECT_EQ(mesh->vertexIndex()[0], meshBack->vertexIndex()[0]);
//  EXPECT_EQ(mesh->vertexIndex()[1], meshBack->vertexIndex()[1]);
//  EXPECT_EQ(mesh->vertexIndex()[2], meshBack->vertexIndex()[2]);
//  EXPECT_EQ(mesh->vertexIndex()[3], meshBack->vertexIndex()[3]);

//  EXPECT_EQ(mesh->stripData()[0], meshBack->stripData()[0]);
//  EXPECT_EQ(mesh->stripData()[1], meshBack->stripData()[1]);
//  EXPECT_EQ(mesh->stripData()[2], meshBack->stripData()[2]);
//  EXPECT_EQ(mesh->stripData()[3], meshBack->stripData()[3]);

//  EXPECT_EQ(mesh->vertexData()[0], meshBack->vertexData()[0]);
//  EXPECT_EQ(mesh->vertexData()[1], meshBack->vertexData()[1]);
//  EXPECT_EQ(mesh->vertexData()[2], meshBack->vertexData()[2]);
//  EXPECT_EQ(mesh->vertexData()[3], meshBack->vertexData()[3]);

//  file::RemoveDir(path);
// }

// TEST_F(Mesh_VertexIndexMeshes, DeleteMesh)
// {
//  utility::UUID uuid;
//  std::string path = file::tempPath() + "/" + uuid.Str() + "/";
//  file::MkDir(path + "meshes/1.000/0/0/0/0");

//  VertexIndexDataSource meshes(path, &uvm);
//  EXPECT_CALL(uvm, Get(_)).Times(1);
//  coords::Mesh meshCoord(coords::Chunk(0,0,0,0), 1);
//  auto mesh = std::make_shared<VertexIndexMesh>(meshCoord);

//  meshes.Put(meshCoord, mesh);
//  EXPECT_EQ(meshes.Get(meshCoord)->coord(), meshCoord);

//  meshes.Put(meshCoord, boost::shared_ptr<VertexIndexMesh>());
//  EXPECT_FALSE(meshes.Get(meshCoord));

//  file::RemoveDir(path);
// }
}
}  // namespace om::test::
