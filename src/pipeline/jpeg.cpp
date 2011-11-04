#include "common/common.h"

#include "pipeline/jpeg.h"

extern "C"
{
#include <jpeglib.h>
#include <jerror.h>
}

namespace om {
namespace pipeline {

template <typename T>
void jpeg<T>::compress(int pixelsize, int jpegsubsamp, uint8_t* data)
{
    std::cout << "Jpegging" << std::endl;
    unsigned long buffSize = TJBUFSIZE(width_, height_);
    compressed_.reset(new char[buffSize]);

    if(tjCompress(handle_,
                  reinterpret_cast<unsigned char*>(data),
                  width_,
                  0,
                  height_,
                  pixelsize,
                  reinterpret_cast<unsigned char*>(compressed_.get()),
                  &buffSize,
                  jpegsubsamp,
                  90, // quality
                  0) != 0) // flags
    {
        throw common::ioException(tjGetErrorStr());
    }

    outSize_ = buffSize;
}

char* jpeg8bit::operator()(uint8_t* data)
{
    compress(1, TJ_GRAYSCALE, data);
    return compressed_.get();
}

char* jpeg32bit::operator()(uint32_t* data)
{
    compress(4, TJ_444, reinterpret_cast<uint8_t*>(data));
    return compressed_.get();
}

}
}
