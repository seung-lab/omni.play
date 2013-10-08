#include "pipeline/jpeg.h"

#include "utility/smartPtr.hpp"

namespace om {
namespace pipeline {

data_var jpeg::compress(int pixelsize, int jpegsubsamp, uint8_t* in) const {
  data<char> out;
  unsigned long buffSize = TJBUFSIZE(width_, height_);
  out.data = utility::smartPtr<char>::MallocNumElements(buffSize);

  if (tjCompress(handle_, reinterpret_cast<unsigned char*>(in), width_, 0,
                 height_, pixelsize,
                 reinterpret_cast<unsigned char*>(out.data.get()), &buffSize,
                 jpegsubsamp, 90,  // quality
                 0) !=
      0)  // flags
      {
    throw ioException(tjGetErrorStr());
  }

  out.size = buffSize;
  return out;
}

}
}
