#pragma once

#include "datalayer/file.h"
#include "datalayer/dataSource.hpp"
#include "mesh/vertexIndex/mesh.h"
#include "mesh/dataSources.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace mesh {

class VertexIndexDataSourceImpl;

class VertexIndexDataSource : public VertexIndexMeshDS {
 public:
  VertexIndexDataSource(file::path volumeRoot,
                        chunk::UniqueValuesDS* uniqueValuesSource);
  ~VertexIndexDataSource();

  virtual std::shared_ptr<VertexIndexMesh> Get(const coords::Mesh& coord,
                                               bool async = false) const;
  virtual bool Put(const coords::Mesh& coord,
                   std::shared_ptr<VertexIndexMesh> mesh, bool async = false);

  inline virtual bool is_cache() const { return false; }

  inline virtual bool is_persisted() const { return true; }

 protected:
  std::unique_ptr<VertexIndexDataSourceImpl> impl_;
};
}
}  // namespace om::mesh::