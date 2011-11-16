#pragma once

#include <stdint.h>
#include "boost/scoped_array.hpp"
#include "pipeline/stage.hpp"
#include "tiles/tileFilters.hpp"
#include "turbojpeg.h"


namespace om {
namespace pipeline {

class jpeg : public stage
{
protected:
    int outSize_;
    uint32_t width_;
    uint32_t height_;
    tjhandle handle_;

public:
    jpeg(uint32_t width, uint32_t height)
        : handle_(tjInitCompress())
        , width_(width)
        , height_(height)
    { }

    ~jpeg()
    {
        tjDestroy(handle_);
    }

    data_var operator()(const data<char>& in) const
    {
        return compress(1, TJ_GRAYSCALE, reinterpret_cast<uint8_t*>(in.data.get()));
    }

    data_var operator()(const data<int8_t>& in) const
    {
        boost::scoped_ptr<uint8_t> casted(new uint8_t[in.size]);
        std::copy(in.data.get(), &in.data[in.size], casted.get());
        return compress(1, TJ_GRAYSCALE, casted.get());
    }

    data_var operator()(const data<uint8_t>& in) const
    {
        return compress(1, TJ_GRAYSCALE, in.data.get());
    }

    data_var operator()(const data<int32_t>& in) const
    {
        boost::scoped_ptr<uint32_t> casted(new uint32_t[in.size]);
        std::copy(in.data.get(), &in.data[in.size], casted.get());
        return compress(4, TJ_444, reinterpret_cast<uint8_t*>(casted.get()));
    }

    data_var operator()(const data<uint32_t>& in) const
    {
        return compress(4, TJ_444,
                        reinterpret_cast<uint8_t*>(in.data.get()));
    }

    data_var operator()(const data<float>& in) const
    {
        tiles::filters<float> filter(128);
        boost::shared_ptr<uint8_t> rescaled = filter.rescaleAndCast<uint8_t>(
            boost::shared_ptr<float>(in.data.get()), 0.0f, 1.0f, 255);
        return compress(1, TJ_GRAYSCALE, rescaled.get());
    }

    data_var compress(int pixelsize, int jpegsubsamp, uint8_t* data) const;

    inline int out_size() const {
        return outSize_;
    }
};

data_var operator>>(const data_var& d, const jpeg& v);

}
}
