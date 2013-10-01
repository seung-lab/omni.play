#pragma once

#include "volume/volume.h"

namespace om {
namespace valid {

class VolumeValid {
 public:
  VolumeValid(const volume::Volume& vol, int level, std::string prefix = "\t")
      : vol_(vol), level_(level), prefix_(prefix) {}

  bool Check() const;

 protected:
  const volume::Volume& vol_;
  const int level_;
  std::string prefix_;
};
}
}  // namespace om::valid::
