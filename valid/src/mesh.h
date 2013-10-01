#pragma once

#include "volume/volume.h"

namespace om {
namespace valid {

class MeshValid {
 public:
  MeshValid(std::string path, const coords::VolumeSystem& system, int level,
            std::string prefix = "\t")
      : path_(path), system_(system), level_(level), prefix_(prefix) {}

  bool Check() const;

 protected:
  const std::string path_;
  const coords::VolumeSystem& system_;
  const int level_;
  std::string prefix_;
};
}
}  // namespace om::valid::
