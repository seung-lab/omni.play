#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include "mesh/mesh.hpp"
#include "coordinates/coordinates.h"
#include "network/http/interface.hpp"

namespace om {
namespace mesh {

struct DegenerateStripMeshImpl;

class DegenerateStripMesh : public IMesh {
 public:
  DegenerateStripMesh(const coords::Mesh& coord);

  void Draw();

  inline std::vector<Vector3f>& Data() { return data_; }
  inline const std::vector<Vector3f>& Data() const { return data_; }

 private:
  std::shared_ptr<DegenerateStripMeshImpl> impl_;
  std::vector<Vector3f> data_;

  PROP_CONST_REF(coords::Mesh, coord);
};
}

size_t size(const mesh::DegenerateStripMesh& m);

namespace network {
namespace http {
template <> struct interface<mesh::DegenerateStripMesh> {
  static std::string mime_type() { return "application/octet-stream"; }
  static std::string serialize(
      std::shared_ptr<mesh::DegenerateStripMesh> data) {
    return std::string(reinterpret_cast<const char*>(data->Data().data()),
                       data->Data().size() * sizeof(Vector3f));
  }
  static std::shared_ptr<mesh::DegenerateStripMesh> deserialize(
      const coords::Mesh& coord, const std::string& data) {
    auto ret = std::make_shared<mesh::DegenerateStripMesh>(coord);
    ret->Data().resize(data.size() / sizeof(Vector3f));
    std::copy(data.begin(), data.end(),
              reinterpret_cast<char*>(ret->Data().data()));
    return ret;
  }
};
}
}

}  // namespace om::mesh::
