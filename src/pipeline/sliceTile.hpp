#pragma once

#include "pipeline/stage.hpp"
#include "chunks/rawChunkSlicer.hpp"

namespace om {
namespace pipeline {

class sliceTile : public stage
{
private:
    common::viewType view_;
    int depth_;
    int chunkSize_;

public:
    sliceTile(common::viewType view, int depth, int chunkSize)
        : view_(view)
        , depth_(depth)
        , chunkSize_(chunkSize)
    { }

    template <typename T>
    data_var operator()(const data<T>& in) const
    {
        data<T> out;
        std::cout << "Executing sliceTile stage." << std::endl;
        chunks::rawChunkSlicer<T> slicer(chunkSize_, in.data.get());
        out.data.reset(slicer.GetCopyOfTile(view_, depth_).get());
        out.size = chunkSize_ * chunkSize_;
        return out;
    }
};

data_var operator>>(const data_var& d, const sliceTile& v) {
    return boost::apply_visitor(v, d);
}

}
}
