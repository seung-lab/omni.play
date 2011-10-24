#include "jpeg/jpeg.h"
#include "common/exception.h"
#include "utility/smartPtr.hpp"

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

namespace om {
namespace jpeg {

writer::writer()
    : handle(tjInitCompress())
{}

writer::~writer() {
    tjDestroy(handle);
}

boost::shared_ptr<char> writer::write(unsigned char * src,
                                      const int width, const int height,
                                      const int pixelFormat, long unsigned int& size)
{
    unsigned char * buff = NULL; // turbojpeg will choose the right size for the buffer.

    tjCompress(handle,
               src,
               width,
               width * pixelFormat,
               height,
               pixelFormat,
               buff,
               &size,
               TJ_GRAYSCALE,
               75, //quality
               0); // flags

    char* err = tjGetErrorStr();
    if (err) {
        throw common::exception("jpegCompressionException", err);
    }

    boost::shared_ptr<char> image =
        utility::smartPtr<char>::MallocNumElements(size, common::DONT_ZERO_FILL);

    std::memcpy(image.get(), buff, size * sizeof(char));

    tjDestroy(buff);

    return image;
}

}
}
