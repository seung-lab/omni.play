#pragma once

#include "precomp.h"
#include "boost/scoped_array.hpp"
#include "pipeline/stage.hpp"
#include "tile/tileFilters.hpp"
#include "turbojpeg.h"

namespace om {
namespace pipeline {

class jpeg : public stage {
 protected:
  int outSize_;
  uint32_t width_;
  uint32_t height_;
  tjhandle handle_;

 public:
  jpeg(uint32_t width, uint32_t height)
      : width_(width), height_(height), handle_(tjInitCompress()) {}

  ~jpeg() { tjDestroy(handle_); }

  data_var operator()(const Data<bool>& in) const {
    return compress(1, TJ_GRAYSCALE, reinterpret_cast<uint8_t*>(in.data.get()));
  }

  data_var operator()(const Data<char>& in) const {
    return compress(1, TJ_GRAYSCALE, reinterpret_cast<uint8_t*>(in.data.get()));
  }

  data_var operator()(const Data<int8_t>& in) const {
    std::unique_ptr<uint8_t> casted(new uint8_t[in.size]);
    std::copy(in.data.get(), &in.data.get()[in.size], casted.get());
    return compress(1, TJ_GRAYSCALE, casted.get());
  }

  data_var operator()(const Data<uint8_t>& in) const {
    return compress(1, TJ_GRAYSCALE, in.data.get());
  }

  data_var operator()(const Data<int32_t>& in) const {
    std::unique_ptr<uint32_t> casted(new uint32_t[in.size]);
    std::copy(in.data.get(), &in.data.get()[in.size], casted.get());
    return compress(4, TJ_444, reinterpret_cast<uint8_t*>(casted.get()));
  }

  data_var operator()(const Data<uint32_t>& in) const {
    return compress(4, TJ_444, reinterpret_cast<uint8_t*>(in.data.get()));
  }

  data_var operator()(const Data<float>& in) const {
    tile::Filters<float> filter(128);
    uint8_t rescaled[128 * 128];
    filter.rescaleAndCast(in.data.get(), 0.0f, 1.0f, 255, &rescaled[0]);
    return compress(1, TJ_GRAYSCALE, &rescaled[0]);
  }

  data_var compress(int pixelsize, int jpegsubsamp, uint8_t* data) const;

  inline int out_size() const { return outSize_; }
};
}
}
