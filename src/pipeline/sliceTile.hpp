#pragma once

#include "pipeline/stage.hpp"
#include "chunks/chunk.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "datalayer/memMappedFile.hpp"

namespace om {
namespace pipeline {

class sliceTile : public stage
{
private:
    common::viewType view_;
    coords::data dc_;
    int chunkSize_;

public:
    sliceTile(common::viewType view, coords::data dc)
        : view_(view)
        , dc_(dc)
        , chunkSize_(dc.volume()->GetChunkDimension())
    { }

    template <typename T>
    data_var operator()(const chunks::Chunk<T>& chunk) const
    {
        data<T> out;
        chunks::rawChunkSlicer<T> slicer(chunkSize_, chunk.ptr());
        out.data = slicer.GetCopyOfTile(view_, dc_.toTileDepth(view_));
        out.size = chunkSize_ * chunkSize_;
        return out;
    }
};

inline data_var operator>>(const chunks::ChunkVar& d, const sliceTile& v) {
    return boost::apply_visitor(v, d);
}

}
}
