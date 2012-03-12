#pragma once

#include "common/common.h"

extern "C" {
#include "turbojpeg.h"
#include "jerror.h"
}

namespace om {
namespace jpeg {

class writer
{
private:
    const tjhandle handle;

public:
    writer();
    ~writer();

    inline boost::shared_ptr<char> write32(char * src, const int width, const int height, int& size)
    {
        return write(src, width, height, 4, size);
    }

    inline boost::shared_ptr<char> write8(char * src, const int width, const int height, int& size)
    {
        return write(src, width, height, 1, size);
    }

private:
    boost::shared_ptr<char> write(char * src, const int width, const int height,
                                  const int pixelFormat, int& size);
};

} // namespace jpeg
} // namespace om

