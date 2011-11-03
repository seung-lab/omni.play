#pragma once

#include "pipeline/stage.hpp"
#include "png.h"

#include <vector>

namespace om {
namespace pipeline {

template<typename T>
class png : stage<T, char>
{
protected:
    std::vector<char> compressed_;
    int outSize_;
    uint32_t width_;
    uint32_t height_;

public:
    png(out_stage<T>* pred, uint32_t width, uint32_t height)
        : stage<T, char>(pred)
        , width_(width)
        , height_(height)
    { }

    ~png()
    { }

    // heavily based on libpng manual http://www.libpng.org/pub/png/libpng-1.4.0-manual.pdf
    // TODO: Error handling
    void compress(const int bit_depth, const int color_type, const uint8_t* data)
    {
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                      NULL,
                                                      NULL, // user error functions
                                                      NULL);
        if (!png_ptr) {
            return;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            return;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return;
        }

        png_set_write_fn(png_ptr, &compressed_,
                         &om::pipeline::png<T>::write_data,
                         &om::pipeline::png<T>::flush_data);

/*
  width - holds the width of the image in pixels (up to 2ˆ31).
  height - holds the height of the image in pixels (up to 2ˆ31).
  bit_depth - holds the bit depth of one of the image channels.
              (valid values are 1, 2, 4, 8, 16 and depend also on the color_type.
              See also significant bits (sBIT) below).
  color_type - describes which color/alpha channels are present.
               PNG_COLOR_TYPE_GRAY (bit depths 1, 2, 4, 8, 16)
               PNG_COLOR_TYPE_GRAY_ALPHA (bit depths 8, 16)
               PNG_COLOR_TYPE_PALETTE (bit depths 1, 2, 4, 8)
               PNG_COLOR_TYPE_RGB (bit_depths 8, 16)
               PNG_COLOR_TYPE_RGB_ALPHA (bit_depths 8, 16)
               PNG_COLOR_MASK_PALETTE
               PNG_COLOR_MASK_COLOR
               PNG_COLOR_MASK_ALPHA
  interlace_type - PNG_INTERLACE_NONE or PNG_INTERLACE_ADAM7
  compression_type - (must be PNG_COMPRESSION_TYPE_DEFAULT)
  filter_method - (must be PNG_FILTER_TYPE_DEFAULT or, if you are writing a PNG to
                  be embedded in a MNG datastream, can also be PNG_INTRAPIXEL_DIFFERENCING)
*/
        png_set_IHDR(png_ptr, info_ptr, width_, height_,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        if(color_type == PNG_COLOR_TYPE_PALETTE ||
           color_type == PNG_COLOR_MASK_PALETTE)
        {
/*
  palette - the palette for the file (array of png_color)
  num_palette - number of entries in the palette
*/
            png_color palette[2];
            palette[0].red = 0; palette[0].green = 0; palette[0].blue = 0;
            palette[1].red = 27; palette[1].green = 224; palette[1].blue = 40;

            png_set_PLTE(png_ptr, info_ptr, palette, 2);

/*
  trans_alpha - array of alpha (transparency) entries for palette (PNG_INFO_tRNS)
  trans_color - graylevel or color sample values (in order red, green, blue) of the
                single transparent color for non-paletted images (PNG_INFO_tRNS)
  num_trans - number of transparent entries (PNG_INFO_tRNS)*/
            uint8_t trans_alpha [] = { 0 };
            png_set_tRNS(png_ptr, info_ptr, trans_alpha, 1, NULL);
        }

/*srgb_intent - the rendering intent (PNG_INFO_sRGB) The presence of
                the sRGB chunk means that the pixel data is in the sRGB color space.
                This chunk also implies specific values of gAMA and cHRM. Rendering
                intent is the CSS-1 property that has been defined by the International
                Color Consortium (http://www.color.org).
                It can be one of
                PNG_sRGB_INTENT_SATURATION,
                PNG_sRGB_INTENT_PERCEPTUAL,
                PNG_sRGB_INTENT_ABSOLUTE, or
                PNG_sRGB_INTENT_RELATIVE.*/
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, PNG_sRGB_INTENT_RELATIVE);

/*
  sig_bit - the number of significant bits for (PNG_INFO_sBIT) each of the gray, red,
            green, and blue channels, whichever are appropriate for the given color type
            (png_color_16)
*/
        png_color_8 sig_bit;
        if(color_type == PNG_COLOR_TYPE_GRAY) {
            sig_bit.gray = bit_depth;
        } else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            sig_bit.gray = bit_depth;
            sig_bit.alpha = bit_depth;
        } else if (color_type == PNG_COLOR_TYPE_RGB) {
            sig_bit.red = bit_depth;
            sig_bit.green = bit_depth;
            sig_bit.blue = bit_depth;
        } else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
            sig_bit.red = bit_depth;
            sig_bit.green = bit_depth;
            sig_bit.blue = bit_depth;
            sig_bit.alpha = bit_depth;
        }

        png_set_sBIT(png_ptr, info_ptr, &sig_bit);

        int pixelBytes = (bit_depth + 7) / 8; // all bit depths are at least 1 byte large

        png_byte* row_pointers[height_];
        for(int i = 0; i < height_; i++) {
            row_pointers[i] =
                const_cast<png_byte*>(
                    reinterpret_cast<const png_byte*>(&data[i * width_ * pixelBytes]));
        }

        png_set_rows(png_ptr, info_ptr, row_pointers);

        int png_transforms = PNG_TRANSFORM_IDENTITY;
        if(bit_depth < 8) {
            png_transforms |= PNG_TRANSFORM_PACKING;
        }

        png_write_png(png_ptr, info_ptr, png_transforms, NULL);

        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    static void write_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        std::vector<char>* io_ptr = reinterpret_cast<std::vector<char>*>(png_get_io_ptr(png_ptr));
        for(int i = 0; i < length; i++) {
            io_ptr->push_back(data[i]);
        }
    }

    static void flush_data(png_structp png_ptr) {}

    inline void cleanup() {
        compressed_.clear();
    }

    inline int out_size() const {
        return compressed_.size();
    }
};

class png8bit : public png<uint8_t>
{
public:
    png8bit(out_stage<uint8_t>* pred, uint32_t width, uint32_t height)
        : png<uint8_t>(pred, width, height)
    {}

    char* operator()(uint8_t* data)
    {
        compress(8, PNG_COLOR_TYPE_GRAY, data);
        return &(*compressed_.begin());
    }
};

class png32bit : public png<uint32_t>
{
public:
    png32bit(out_stage<uint32_t>* pred, uint32_t width, uint32_t height)
        : png<uint32_t>(pred, width, height)
    {}

    char* operator()(uint32_t* data)
    {
        compress(8, PNG_COLOR_TYPE_RGB_ALPHA, reinterpret_cast<uint8_t*>(data));
        return &(*compressed_.begin());
    }
};

class pngMask : public png<uint8_t>
{
public:
    pngMask(out_stage<uint8_t>* pred, uint32_t width, uint32_t height)
        : png<uint8_t>(pred, width, height)
    {}

    char* operator()(uint8_t* data)
    {
        compress(1, PNG_COLOR_TYPE_GRAY, data);
        return &(*compressed_.begin());
    }
};



}
}
