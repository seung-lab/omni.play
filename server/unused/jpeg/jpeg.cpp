#include "jpeg/jpeg.h"
#include "common/exception.h"
#include "utility/smartPtr.hpp"

namespace om {
namespace jpeg {

writer::writer()
    : handle(tjInitCompress())
{}

writer::~writer() {
    tjDestroy(handle);
}

boost::shared_ptr<char> writer::write(char * src, const int width, const int height,
                                      const int pixelFormat, int& size)
{
    long unsigned int longSize;
    char* buffer = (char*)malloc(TJBUFSIZE(width, height) * sizeof(char));

    if(tjCompress(handle,
                  reinterpret_cast<unsigned char*>(src),
                  width,
                  width * pixelFormat,
                  height,
                  pixelFormat,
                  reinterpret_cast<unsigned char*>(buffer),
                  &longSize,
                  TJ_GRAYSCALE,
                  75, //quality
                  0) == -1) // flags
    {
        char* err = tjGetErrorStr();
        throw common::exception("jpegCompressionException", err);
    }

    size = longSize;

    return utility::smartPtr<char>::WrapMalloc(buffer);
}

}
}
