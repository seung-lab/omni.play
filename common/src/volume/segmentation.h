#pragma once

#include "volume/volume.h"
#include "volume/iterators.hpp"
#include "volume/isegmentation.hpp"
#include "datalayer/paths.hpp"

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
  Segmentation(file::Paths p, uint8_t id, std::string username = "_default");
  ~Segmentation();

  mesh::VertexIndexMeshDS& MeshDS() const;
  chunk::UniqueValuesDS& UniqueValuesDS() const;
  segment::SegDataVector& SegData() const;
  segment::SegListDataVector& SegListData() const;
  segment::EdgeVector& Edges() const;

  template <typename T>
  struct filtered_iterable_volume {
    chunk_filtered_dataval_iterator<T> begin();
    chunk_filtered_dataval_iterator<T> end();
    Segmentation& vol;
    coords::DataBbox bounds;
    T id;
  };

  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id);
  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id, coords::GlobalBbox bounds);
  template <typename T>
  filtered_iterable_volume<T> SegIterate(T id, coords::DataBbox bounds);

  template <typename T>
  struct filtered_set_iterable_volume {
    chunk_filtered_dataval_iterator<T> begin();
    chunk_filtered_dataval_iterator<T> end();
    Segmentation& vol;
    coords::DataBbox bounds;
    std::set<T> ids;
  };

  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(std::set<T> ids);
  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(std::set<T> ids,
                                             coords::GlobalBbox bounds);
  template <typename T>
  filtered_set_iterable_volume<T> SegIterate(std::set<T> ids,
                                             coords::DataBbox bounds);

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
