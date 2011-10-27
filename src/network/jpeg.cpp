#include "network/jpeg.h"
#include "common/exception.h"

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

extern "C"
{
#include <jpeglib.h>
#include <jerror.h>
}

// from https://github.com/LaurentGomila/SFML/blob/cb1f9385825c4645dae1c233684699c6fcb6c0a7/src/SFML/Graphics/ImageLoader.cpp
void om::jpeg::writeRGB(const uint32_t width, const uint32_t height,
                     uint8_t const*const pixels, const std::string& filename)
{
    // Open the file to write in
    FILE* file = fopen(filename.c_str(), "wb");
    if (!file){
        throw common::ioException("could not create file");
    }

    // Initialize the error handlers
    jpeg_compress_struct compressInfos;
    jpeg_error_mgr errorManager;
    compressInfos.err = jpeg_std_error(&errorManager);

    // Initialize all the writing and compression infos
    jpeg_create_compress(&compressInfos);
    compressInfos.image_width      = width;
    compressInfos.image_height     = height;
    compressInfos.input_components = 3;
    compressInfos.in_color_space   = JCS_RGB;
    jpeg_stdio_dest(&compressInfos, file);
    jpeg_set_defaults(&compressInfos);
    jpeg_set_quality(&compressInfos, 90, false);

    // Get rid of the aplha channel
    boost::scoped_ptr<uint8_t> bufferPtr(new uint8_t[width * height * 3]);
    uint8_t* ptr = bufferPtr.get();

    for (uint64_t i = 0; i < width * height; ++i)
    {
        ptr[i * 3 + 0] = pixels[i * 4 + 1];  // index 0 would be alpha
        ptr[i * 3 + 1] = pixels[i * 4 + 2];
        ptr[i * 3 + 2] = pixels[i * 4 + 3];
    }

    // Start compression
    jpeg_start_compress(&compressInfos, true);

    // Write each row of the image_height
    while (compressInfos.next_scanline < compressInfos.image_height)
    {
        JSAMPROW rawPointer = ptr + (compressInfos.next_scanline * width * 3);
        jpeg_write_scanlines(&compressInfos, &rawPointer, 1);
    }

    // Finish compression
    jpeg_finish_compress(&compressInfos);
    jpeg_destroy_compress(&compressInfos);

    fclose(file);
}

void om::jpeg::write8bit(const uint32_t width, const uint32_t height,
                         uint8_t const*const pixels, const std::string& filename)
{
    // Open the file to write in
    FILE* file = fopen(filename.c_str(), "wb");
    if (!file){
        throw common::ioException("could not create file");
    }

    // Initialize the error handlers
    jpeg_compress_struct compressInfos;
    jpeg_error_mgr errorManager;
    compressInfos.err = jpeg_std_error(&errorManager);

    // Initialize all the writing and compression infos
    jpeg_create_compress(&compressInfos);
    compressInfos.image_width      = width;
    compressInfos.image_height     = height;
    compressInfos.input_components = 1;
    compressInfos.in_color_space   = JCS_GRAYSCALE;
    jpeg_stdio_dest(&compressInfos, file);
    jpeg_set_defaults(&compressInfos);
    jpeg_set_quality(&compressInfos, 50, true);

    uint8_t* ptr = const_cast<uint8_t*>(pixels);

    // Start compression
    jpeg_start_compress(&compressInfos, true);

    // Write each row of the image_height
    while (compressInfos.next_scanline < compressInfos.image_height)
    {
        JSAMPROW rawPointer = ptr + (compressInfos.next_scanline * width);
        jpeg_write_scanlines(&compressInfos, &rawPointer, 1);
    }

    // Finish compression
    jpeg_finish_compress(&compressInfos);
    jpeg_destroy_compress(&compressInfos);

    fclose(file);
}
