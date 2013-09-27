#include "volume/segmentation.h"
#include "volume/metadataManager.h"
#include "chunk/cachedUniqueValuesDataSource.hpp"
#include "mesh/vertexIndex/cachedDataSource.hpp"
#include "segment/fileDataSource.hpp"

namespace om {
namespace volume {

Segmentation::Segmentation(file::path uri)
    : Volume(uri),
      uvmDS_(new chunk::CachedUniqueValuesDataSource(uri)),
      meshDS_(new mesh::CachedDataSource(uri, uvmDS_.get())),
      segDataDS_(new segment::FileDataSource(uri)),
      segData_(new segment::SegDataVector(*segDataDS_, segment::PageSize,
                                          Metadata().numSegments() + 1)),
      segListDataDS_(new segment::ListTypeFileDataSource(uri)),
      segListData_(new segment::SegListDataVector(
          *segListDataDS_, segment::PageSize, Metadata().numSegments() + 1)),
      mst_(new segment::EdgeVector(uri / file::Paths::Seg::MST())) {}

Segmentation::~Segmentation() {}

mesh::VertexIndexMeshDS& Segmentation::MeshDS() const { return *meshDS_; }
chunk::UniqueValuesDS& Segmentation::UniqueValuesDS() const { return *uvmDS_; }
segment::SegDataVector& Segmentation::SegData() const { return *segData_; }
segment::SegListDataVector& Segmentation::SegListData() const {
  return *segListData_;
}
segment::EdgeVector& Segmentation::Edges() const { return *mst_; }
}
}