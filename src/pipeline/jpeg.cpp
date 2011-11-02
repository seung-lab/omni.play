#include "common/common.h"

#include "pipeline/jpeg.h"

extern "C"
{
#include <jpeglib.h>
#include <jerror.h>
}

namespace om {
namespace pipeline {

// from https://github.com/LaurentGomila/SFML/blob/cb1f9385825c4645dae1c233684699c6fcb6c0a7/src/SFML/Graphics/ImageLoader.cpp
template <typename T>
void jpeg<T>::compress(int pixelsize, int jpegsubsamp, uint8_t* data)
{
    unsigned long buffSize = TJBUFSIZE(width_, height_);
    compressed_.reset(new char[buffSize]);

    if(tjCompress(handle_,
                  reinterpret_cast<unsigned char*>(data),
                  width_,
                  width_ * pixelsize,
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
/*
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
    compressInfos.input_components = input_components;
    compressInfos.in_color_space   = in_color_space;
    jpeg_stdio_dest(&compressInfos, file);
    jpeg_set_defaults(&compressInfos);
    jpeg_set_quality(&compressInfos, 90, false);

    // Start compression
    jpeg_start_compress(&compressInfos, true);

    // Write each row of the image_height
    while (compressInfos.next_scanline < compressInfos.image_height)
    {
        JSAMPROW rawPointer = ptr + (compressInfos.next_scanline * width * input_components);
        jpeg_write_scanlines(&compressInfos, &rawPointer, 1);
    }

    // Finish compression
    jpeg_finish_compress(&compressInfos);
    jpeg_destroy_compress(&compressInfos);

    fclose(file);
*/
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
