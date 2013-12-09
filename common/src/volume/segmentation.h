#pragma once

#include "volume/volume.h"
#include "volume/isegmentation.hpp"

namespace om {
namespace mesh {
class CachedDataSource;
}
namespace chunk {
class CachedUniqueValuesDataSource;
}
namespace segment {
class FileDataSource;
class ListTypeFileDataSource;
}

namespace volume {

class Segmentation : public Volume, virtual public ISegmentation {
 public:
  Segmentation(file::path uri);
  ~Segmentation();

  mesh::VertexIndexMeshDS& MeshDS() const;
  chunk::UniqueValuesDS& UniqueValuesDS() const;
  segment::SegDataVector& SegData() const;
  segment::SegListDataVector& SegListData() const;
  segment::EdgeVector& Edges() const;

 private:
  std::unique_ptr<chunk::CachedUniqueValuesDataSource> uvmDS_;
  std::unique_ptr<mesh::CachedDataSource> meshDS_;
  std::unique_ptr<segment::FileDataSource> segDataDS_;
  std::unique_ptr<segment::SegDataVector> segData_;
  std::unique_ptr<segment::ListTypeFileDataSource> segListDataDS_;
  std::unique_ptr<segment::SegListDataVector> segListData_;
  std::unique_ptr<segment::EdgeVector> mst_;
};
}
}
