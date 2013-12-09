#include "mesh.h"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include <cmath>
#include "controller.hpp"

namespace om {
namespace valid {

bool detail(const mesh::VertexIndexMesh& mesh,
            const coords::VolumeSystem& system, std::string prefix) {
  {
    Controller::Context c("VertexData");
    const float* vertexData = mesh.Data().VertexData();
    VALID_TEST(mesh.Data().VertexDataCount() > 0, "No VertexData");

    VALID_FOR(i, 0, mesh.Data().VertexDataCount(), 3) {
      coords::Norm vertex(vertexData[i], vertexData[i + 1], vertexData[i + 2],
                          system);
      VALID_TEST(-1 <= vertex.x && vertex.x <= 1, vertex);
      VALID_TEST(-1 <= vertex.y && vertex.y <= 1, vertex);
      VALID_TEST(-1 <= vertex.z && vertex.z <= 1, vertex);
      VALID_MULTITEST(vertex.length() > 1e-4f, vertex, 3);
    }
  }

  {
    Controller::Context c("VertexIndex");

    const uint32_t* indexData = mesh.Data().VertexIndex();
    VALID_TEST(mesh.Data().VertexIndexCount() > 0, "No IndexData");

    VALID_FOR(i, 0, mesh.Data().VertexIndexCount(), 1) {
      VALID_TEST(
          0 <= indexData[i] && indexData[i] < mesh.Data().VertexIndexCount(),
          "Bad Index");
    }
  }

  {
    Controller::Context c("StripData");

    const uint32_t* stripData = mesh.Data().StripData();
    VALID_TEST(mesh.Data().StripDataCount() > 0, "No StripData");

    VALID_FOR(i, 0, mesh.Data().StripDataCount(), 1) {
      VALID_TEST(
          0 <= stripData[i] && stripData[i] < mesh.Data().VertexIndexCount(),
          stripData[i]);
    }
  }

  return true;
}

bool MeshValid::Check() const {
  chunk::UniqueValuesFileDataSource uvDS(path_);
  mesh::VertexIndexDataSource meshDS(path_, &uvDS);

  std::shared_ptr<std::vector<coords::Chunk> > chunks =
      system_.MipChunkCoords();

  VALID_FOR(i, 0, chunks->size(), 1) {
    coords::Chunk cc = (*chunks)[i];
    Controller::Context c(cc);
    std::shared_ptr<chunk::UniqueValues> uv = uvDS.Get(cc);
    if (!uv) {
      continue;
    }

    std::vector<uint32_t> values = uv->Values;
    VALID_FOR(j, 0, values.size(), 1) {
      uint32_t segId = values[j];
      Controller::Context c2(segId);

      coords::Mesh mc(cc, segId);
      std::shared_ptr<mesh::VertexIndexMesh> mesh = meshDS.Get(mc);

      VALID_TEST((bool) mesh, "Mesh Does Not Exist!");

      if (level_ > 1) {
        VALID_TEST(detail(*mesh, system_, prefix_), "Bad Mesh");
      }
    }
  }
  return true;
}
}
}  // namespace om::valid::