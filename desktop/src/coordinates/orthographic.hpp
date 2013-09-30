#pragma once

#include "../../include/vmmlib/vmmlib.h"

// orthographic projection matrix
// http://en.wikipedia.org/wiki/Orthographic_projection_(geometry)

namespace om {

class orthoProjection {
 private:
  float right_;
  float left_;
  float top_;
  float bottom_;
  float near_;
  float far_;
  float scale_;

  vmml::Matrix4f project_;
  vmml::Matrix4f unProject_;

 public:
  orthoProjection(float right, float left, float top, float bottom, float near,
                  float far, float scale)
      : right_(right),
        left_(left),
        top_(top),
        bottom_(bottom),
        near_(near),
        far_(far),
        scale_(scale) {
    compute();
  }

 private:
  void compute() {
    project_.m00 = 1.0f / (right_ - left_);
    project_.m11 = 1.0f / (top_ - bottom_);
    project_.m22 = -1.0f / (far_ - near_);

    project_.m03 = project_.m00 * -1 * (right_ + left_);
    project_.m13 = project_.m11 * -1 * (top_ + bottom_);
    project_.m23 = project_.m22 * -1 * (far_ + near_);

    project_.m00 *= 2;
    project_.m11 *= 2;
    project_.m22 *= 2;

    project_.scaleTranslation(1.0f / scale_);
    project_.m33 = scale_;
  }
};

}  // namespace om