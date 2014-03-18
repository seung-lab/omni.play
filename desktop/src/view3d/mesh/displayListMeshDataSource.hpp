#pragma once
#include "precomp.h"

#include "datalayer/dataSource.hpp"
#include "mesh/dataSources.hpp"
#include "view3d/mesh/displayListMesh.hpp"
#include "view3d/mesh/displayListMeshDataSources.hpp"

class QGLContext;

namespace om {
namespace mesh {

struct DisplayListMeshCoord {
  coords::Mesh Coord;
  const QGLContext* Context;

  friend bool operator==(const DisplayListMeshCoord& c1,
                         const DisplayListMeshCoord& c2) {
    return c1.Coord == c2.Coord && c1.Context == c2.Context;
  }

  const std::string keyStr() const { return Coord.keyStr(); }
};

inline std::ostream& operator<<(std::ostream& out,
                                const DisplayListMeshCoord& c) {
  return out << c.Context << ":" << c.Coord;
}

template <typename TMesh>
class DisplayListMeshDataSource : public DisplayListMeshDS {
 public:
  DisplayListMeshDataSource(VertexIndexMeshDS& meshDataSource)
      : meshDataSource_(meshDataSource) {}

  std::shared_ptr<DisplayListMesh> Get(const DisplayListMeshCoord& coord,
                                       bool async = false) {
    std::shared_ptr<TMesh> raw = meshDataSource_.Get(coord.Coord, async);
    if (!raw) {
      return std::shared_ptr<DisplayListMesh>();
    }
    return std::make_shared<DisplayListMesh>(raw, coord.Context);
  }

  bool Put(const DisplayListMeshCoord&, std::shared_ptr<DisplayListMesh>,
           bool asnyc = false) {
    throw om::NotImplementedException("fixme");
  }

  bool is_cache() const { return false; }

  bool is_persisted() const { return false; }

 private:
  IDataSource<coords::Mesh, TMesh>& meshDataSource_;
};
}
}  // namespace om::mesh::

namespace std {
template <>
struct hash<om::mesh::DisplayListMeshCoord> {
  size_t operator()(const om::mesh::DisplayListMeshCoord& m) const {
    std::size_t h1 = std::hash<om::coords::Mesh>()(m.Coord);
    std::size_t h2 = std::hash<size_t>()((size_t)m.Context);
    return h1 ^ (h2 << 1);
  }
};
}
