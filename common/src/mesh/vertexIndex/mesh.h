#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include "mesh/mesh.hpp"
#include "mesh/vertexIndex/data.hpp"
#include "coordinates/coordinates.h"

namespace om {
namespace mesh {

struct VertexIndexMeshImpl;

class VertexIndexMesh : public IMesh {
 public:
  VertexIndexMesh(const coords::Mesh& coord);

  void Draw();

  inline bool HasData() const { return data_.HasData(); }

  inline void HasData(bool has) { data_.HasData(has); }

  inline uint64_t NumBytes() const { return data_.NumBytes(); }

  inline VertexIndexData& Data() { return data_; }

  inline const VertexIndexData& Data() const { return data_; }

 private:
  std::shared_ptr<VertexIndexMeshImpl> impl_;
  VertexIndexData data_;

  PROP_CONST_REF(coords::Mesh, coord);
};
}

size_t size(const mesh::VertexIndexMesh& m);

}  // namespace om::mesh::
