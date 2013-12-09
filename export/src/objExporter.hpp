#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "iMeshExporter.hpp"

namespace om {
namespace exporter {

class Obj : public IMeshExporter {
 public:

  Obj(const coords::VolumeSystem& coords, double scale)
      : coords_(coords), scale_(scale) {
    Matrix4f invTransform;
    if (!coords.NormToGlobalMat().getInverse(invTransform)) {
      throw ArgException("Transform is not invertible.");
    }
    normTransform_ = invTransform.getTransposed();
  }

  void Accumulate(const float* points, const size_t points_length,
                  const uint32_t* indices, const size_t indices_length,
                  const uint32_t* strips, const size_t strips_length) {
    size_t current = points_.size() + 1;
    for (size_t i = 0; i < points_length; i += 6) {
      points_.push_back(coords::Norm(points[i], points[i + 1], points[i + 2],
                                     coords_).ToGlobal() *
                        scale_);
      Vector3f normal = normTransform_ * Vector4f(points[i + 3], points[i + 4],
                                                  points[i + 5], 1);
      normals_.push_back(normal.getNormalized());
    }

    for (size_t i = 0; i < strips_length; i += 2) {
      if (strips[i] == strips[i + 1]) {
        continue;
      }

      bool even = true;
      for (uint32_t j = strips[i]; j < strips[i] + strips[i + 1] - 2; ++j) {
        if (even) {
          faces_.push_back(Vector3i(indices[j] + current,
                                    indices[j + 1] + current,
                                    indices[j + 2] + current));
        } else {
          faces_.push_back(Vector3i(indices[j + 2] + current,
                                    indices[j + 1] + current,
                                    indices[j] + current));
        }
        even = !even;
      }
    }
  }

  std::string Write() {
    std::stringstream ss;

    for (auto& point : points_) {
      ss << "v " << point.x << ' ' << point.y << ' ' << point.z << '\n';
    }

    for (auto& normal : normals_) {
      ss << "vn " << normal.x << ' ' << normal.y << ' ' << normal.z << '\n';
    }

    for (auto& face : faces_) {
      ss << "f " << face[0] << "//" << face[0] << ' ' << face[1] << "//"
         << face[1] << ' ' << face[2] << "//" << face[2] << '\n';
    }
    return ss.str();
  }

 private:
  const coords::VolumeSystem& coords_;
  Matrix4f normTransform_;
  std::vector<coords::Global> points_;
  std::vector<coords::Global> normals_;
  std::vector<Vector3i> faces_;
  const double scale_;
};
}
}  // namespace om::exporter::