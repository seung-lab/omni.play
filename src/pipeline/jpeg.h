#pragma once

#include <stdint.h>
#include "boost/scoped_array.hpp"
#include "pipeline/stage.hpp"
#include "turbojpeg.h"


namespace om {
namespace pipeline {

template <typename T>
class jpeg : public stage<T, char>
{
protected:
    boost::scoped_array<char> compressed_;
    int outSize_;
    uint32_t width_;
    uint32_t height_;
    tjhandle handle_;

public:
    jpeg(out_stage<T>* pred, uint32_t width, uint32_t height)
        : stage<T, char>(pred)
        , handle_(tjInitCompress())
        , width_(width)
        , height_(height)
    { }

    ~jpeg()
    {
        tjDestroy(handle_);
    }

    void compress(int pixelsize, int jpegsubsamp, uint8_t* data);

    inline void cleanup() {
        compressed_.reset();
    }

    inline int out_size() {
        return outSize_;
    }
};

class jpeg8bit : public jpeg<uint8_t>
{
public:
    jpeg8bit(pred_t* pred, uint32_t width, uint32_t height)
        : jpeg<uint8_t>(pred, width, height)
    { }

    char* operator()(uint8_t*);
};

class jpeg32bit : public jpeg<uint32_t>
{
public:
    jpeg32bit(pred_t* pred, uint32_t width, uint32_t height)
        : jpeg<uint32_t>(pred, width, height)
    { }

    char* operator()(uint32_t*);
};

}
}
