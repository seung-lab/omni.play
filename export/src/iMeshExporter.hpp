#pragma once

namespace om {
namespace exporter {

class IMeshExporter {
 public:
  virtual void Accumulate(const float* points, const size_t points_length,
                          const uint32_t* indices, const size_t indices_length,
                          const uint32_t* strips,
                          const size_t strips_length) = 0;
  virtual std::string Write() = 0;
};

}
}