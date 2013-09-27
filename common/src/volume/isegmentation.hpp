#pragma once

#include "common/common.h"
#include "chunk/dataSources.hpp"
#include "mesh/dataSources.hpp"
#include "segment/dataSources.hpp"
#include "volume/ivolume.hpp"

namespace om {
namespace volume {

class ISegmentation : virtual public IVolume {
 public:
  virtual mesh::VertexIndexMeshDS& MeshDS() const = 0;
  virtual chunk::UniqueValuesDS& UniqueValuesDS() const = 0;
  virtual segment::SegDataVector& SegData() const = 0;
  virtual segment::SegListDataVector& SegListData() const = 0;
  virtual segment::EdgeVector& Edges() const = 0;
  // groups
  // annotations
};
}
}  // namespace om::volume::