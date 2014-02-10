#include "volume/segmentation.h"
#include "volume/metadataManager.h"
#include "chunk/cachedUniqueValuesDataSource.hpp"
#include "mesh/vertexIndex/cachedDataSource.hpp"
#include "segment/fileDataSource.hpp"

namespace om {
namespace volume {

Segmentation::Segmentation(file::Paths p, uint8_t id, std::string username)
    : Volume(p.Segmentation(id)),
      uvmDS_(new chunk::CachedUniqueValuesDataSource(p.Segmentation(id))),
      meshDS_(new mesh::CachedDataSource(p.Segmentation(id).string(),
                                         uvmDS_.get())),
      segDataDS_(new segment::FileDataSource(p.UserSegments(username, id))),
      segData_(new segment::SegDataVector(*segDataDS_, segment::PageSize,
                                          Metadata().numSegments() + 1)),
      segListDataDS_(
          new segment::ListTypeFileDataSource(p.UserSegments(username, id))),
      segListData_(new segment::SegListDataVector(
          *segListDataDS_, segment::PageSize, Metadata().numSegments() + 1)),
      mst_(new segment::EdgeVector(p.UserMST(username, id))) {}

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