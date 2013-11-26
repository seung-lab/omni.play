#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"

namespace om {
namespace exporter {

class MSTExporter {
 public:
  MSTExporter(const volume::Segmentation& seg) : seg_(seg) {}

  int Export() {}

 private:
  const volume::Segmentation& seg_;
};
}
}  // namespace om::exporter::