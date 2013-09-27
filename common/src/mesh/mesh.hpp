#pragma once

namespace om {
namespace mesh {

class IMesh {
 public:
  virtual ~IMesh() {}
  virtual void Draw() = 0;
};
}
}  // namespace om::mesh::