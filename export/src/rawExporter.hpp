#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "iMeshExporter.hpp"
#include "zi/assert.hpp"

namespace om {
namespace exporter {

class Raw : public IMeshExporter {
 public:
  void Accumulate(const float* points, const size_t points_length,
                  const uint32_t* indices, const size_t indices_length,
                  const uint32_t* strips, const size_t strips_length) {
    ZI_ASSERT(points_length % 6 == 0);

    for (std::size_t i = 0; i < strips_length; i += 2) {
      if (i > 0) {
        for (uint32_t k = 0; k < 6; ++k) {
          points_.push_back(
              points[indices[strips[i - 2] + strips[i - 1] - 1] * 6 + k]);
        }

        if ((points_.size() / 6) % 2 == 0) {
          for (uint32_t k = 0; k < 6; ++k) {
            points_.push_back(points[indices[strips[i]] * 6 + k]);
          }
        }

        for (uint32_t k = 0; k < 6; ++k) {
          points_.push_back(points[indices[strips[i]] * 6 + k]);
        }
      }

      for (uint32_t j = strips[i]; j < strips[i] + strips[i + 1]; ++j) {
        for (uint32_t k = 0; k < 6; ++k) {
          points_.push_back(points[indices[j] * 6 + k]);
        }
      }

      if (i + 3 < strips_length && strips[i + 2] == strips[i + 3] &&
          strips[i + 3] == 0) {
        continue;
      }
    }
  }

  void Write(std::ostream& out) {
    out.write((char*)&points_.front(), points_.size() * sizeof(float));
  }

 private:
  std::vector<float> points_;
};
}
}  // namespace om::exporter::