#pragma once

#include "volume/segmentation.h"

namespace om {
namespace valid {

class UniqueValuesValid {
 public:
  UniqueValuesValid(const volume::Segmentation& vol, int level,
                    std::string prefix = "\t")
      : vol_(vol), level_(level), prefix_(prefix) {}

  bool Check() const;

 protected:
  const volume::Segmentation& vol_;
  const int level_;
  std::string prefix_;
};
}
}  // namespace om::valid::
