#include "pipeline/jpeg.h"

namespace om {
namespace pipeline {

data_var jpeg::compress(int pixelsize, int jpegsubsamp, uint8_t* in) const {
  unsigned long buffSize = TJBUFSIZE(width_, height_);
  Data<char> out(buffSize);

  if (tjCompress(handle_, reinterpret_cast<unsigned char*>(in), width_, 0,
                 height_, pixelsize,
                 reinterpret_cast<unsigned char*>(out.data.get()), &buffSize,
                 jpegsubsamp, 90,  // quality
                 0) !=
      0)  // flags
      {
    throw IoException(tjGetErrorStr());
  }
  out.size = buffSize;

  return out;
}
}
}
