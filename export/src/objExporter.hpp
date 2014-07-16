#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "iMeshExporter.hpp"

#include <unordered_map>

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
    std::vector<uint32_t> transformed_indicies(indices,
                                               indices + indices_length);
    std::unordered_map<size_t, size_t> lookup_index;

    size_t hit = 0;
    for (size_t i = 0; i < points_length; i += 6) {
      coords::Norm norm(points[i], points[i + 1], points[i + 2], coords_);

      // Deduplicate verticies
      auto iter = lookup_vertex.find(norm);
      if (iter != lookup_vertex.end()) {
        hit++;
        lookup_index[i / 6] = iter->second;
      } else {
        points_.push_back(norm.ToGlobal() * scale_);
        Vector3f normal =
            normTransform_ *
            Vector4f(points[i + 3], points[i + 4], points[i + 5], 1);
        normals_.push_back(normal.getNormalized());

        auto idx = points_.size();
        lookup_vertex[norm] = idx;
        lookup_index[i / 6] = idx;
      }
    }

    for (size_t i = 0; i < strips_length; i += 2) {
      if (strips[i] == strips[i + 1]) {
        continue;
      }

      bool even = true;
      for (uint32_t j = strips[i]; j < strips[i] + strips[i + 1] - 2; ++j) {
        auto a = lookup_index[indices[j]];
        auto b = lookup_index[indices[j + 1]];
        auto c = lookup_index[indices[j + 2]];
        if (a == 0 || b == 0 || c == 0) {
          log_debugs << indices[j] << "/" << indices[j + 1] << "/"
                     << indices[j + 2] << " = " << a << "/" << b << "/" << c;
        }
        if (even) {
          faces_.push_back(Vector3i(a, b, c));
        } else {
          faces_.push_back(Vector3i(c, b, a));
        }
        even = !even;
      }
    }
  }

  void Write(std::ostream& out) override {
    for (auto& point : points_) {
      out << "v " << point.x << ' ' << point.y << ' ' << point.z << '\n';
    }

    for (auto& normal : normals_) {
      out << "vn " << normal.x << ' ' << normal.y << ' ' << normal.z << '\n';
    }

    for (auto& face : faces_) {
      out << "f " << face[0] << "//" << face[0] << ' ' << face[1] << "//"
          << face[1] << ' ' << face[2] << "//" << face[2] << '\n';
    }
  }

 private:
  const coords::VolumeSystem& coords_;
  Matrix4f normTransform_;
  std::vector<coords::Global> points_;
  std::vector<coords::Global> normals_;
  std::vector<Vector3i> faces_;
  std::unordered_map<coords::Norm, size_t> lookup_vertex;
  const double scale_;
};
}
}  // namespace om::exporter::