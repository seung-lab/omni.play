#pragma once

namespace om {
namespace v3d {

class PercDone {
 private:
  uint32_t numMeshesNeeded_;
  uint32_t numMeshesDrawn_;
  uint32_t numMeshesMissing_;

 public:
  void Reset(uint32_t numMeshesNeeded) {
    numMeshesNeeded_ = numMeshesNeeded;
    numMeshesDrawn_ = 0;
    numMeshesMissing_ = 0;
  }

  template <class T>
  void justDrew(T mesh) {
    ++numMeshesDrawn_;
    // TODO: could count number of triangles drawn here...
  }

  void missingMesh() { ++numMeshesMissing_; }

  bool needsRedraw() { return numMeshesMissing_ > 0; }

  float percentDone() {
    if (!numMeshesNeeded_ || !numMeshesDrawn_) {
      return 0;
    }
    return static_cast<float>(numMeshesDrawn_) /
           static_cast<float>(numMeshesNeeded_);
  }

  bool shouldShow() { return numMeshesNeeded_ > 0; }

  bool noMeshData() {
    // TODO: figure out a better way of determing no mesh data exists
    return 0 == numMeshesDrawn_;
  }
};
}
}  // om::v3d::
