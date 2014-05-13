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
      segDataDS_(
          new segment::FileDataSource(p.UserPaths(username).Segments(id))),
      segData_(new segment::SegDataVector(*segDataDS_, segment::PageSize,
                                          Metadata().numSegments() + 1)),
      segListDataDS_(new segment::ListTypeFileDataSource(
          p.UserPaths(username).Segments(id))),
      segListData_(new segment::SegListDataVector(
          *segListDataDS_, segment::PageSize, Metadata().numSegments() + 1)),
      mst_(new segment::EdgeVector(p.UserPaths(username).MST(id))) {}

Segmentation::~Segmentation() {}

mesh::VertexIndexMeshDS& Segmentation::MeshDS() const { return *meshDS_; }
chunk::UniqueValuesDS& Segmentation::UniqueValuesDS() const { return *uvmDS_; }
segment::SegDataVector& Segmentation::SegData() const { return *segData_; }
segment::SegListDataVector& Segmentation::SegListData() const {
  return *segListData_;
}
segment::EdgeVector& Segmentation::Edges() const { return *mst_; }

template <typename T>
chunk_filtered_dataval_iterator<T>
Segmentation::filtered_iterable_volume<T>::begin() {
  return make_chunk_filtered_dataval_iterator(bounds, vol.ChunkDS(),
                                              vol.UniqueValuesDS(), id);
}
template <typename T>
chunk_filtered_dataval_iterator<T>
Segmentation::filtered_iterable_volume<T>::end() {
  return chunk_filtered_dataval_iterator<T>();
}

template <typename T>
Segmentation::filtered_iterable_volume<T> Segmentation::SegIterate(T id) {
  return Segmentation::filtered_iterable_volume<T>{
      *this, metaManager_->bounds().ToDataBbox(Coords(), 0), id};
}
template <typename T>
Segmentation::filtered_iterable_volume<T> Segmentation::SegIterate(
    T id, coords::GlobalBbox bounds) {
  return Segmentation::filtered_iterable_volume<T>{
      *this, bounds.ToDataBbox(Coords(), 0), id};
}
template <typename T>
Segmentation::filtered_iterable_volume<T> Segmentation::SegIterate(
    T id, coords::DataBbox bounds) {
  return Segmentation::filtered_iterable_volume<T>{*this, bounds, id};
}

template <typename T>
chunk_filtered_dataval_iterator<T>
Segmentation::filtered_set_iterable_volume<T>::begin() {
  return make_chunk_filtered_dataval_iterator(bounds, vol.ChunkDS(),
                                              vol.UniqueValuesDS(), ids);
}
template <typename T>
chunk_filtered_dataval_iterator<T>
Segmentation::filtered_set_iterable_volume<T>::end() {
  return chunk_filtered_dataval_iterator<T>();
}

template <typename T>
Segmentation::filtered_set_iterable_volume<T> Segmentation::SegIterate(
    std::set<T> ids) {
  return Segmentation::filtered_set_iterable_volume<T>{
      *this, metaManager_->bounds().ToDataBbox(Coords(), 0), ids};
}
template <typename T>
Segmentation::filtered_set_iterable_volume<T> Segmentation::SegIterate(
    std::set<T> ids, coords::GlobalBbox bounds) {
  return Segmentation::filtered_set_iterable_volume<T>{
      *this, bounds.ToDataBbox(Coords(), 0), ids};
}
template <typename T>
Segmentation::filtered_set_iterable_volume<T> Segmentation::SegIterate(
    std::set<T> ids, coords::DataBbox bounds) {
  return Segmentation::filtered_set_iterable_volume<T>{*this, bounds, ids};
}
}
}