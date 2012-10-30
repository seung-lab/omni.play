#pragma once

#include "pipeline/stage.hpp"
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
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        coords::Chunk cc = dc_.toChunk();
        uint64_t offset = cc.PtrOffset(dc_.volume(), sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);
        chunks::rawChunkSlicer<T> slicer(chunkSize_, chunkPtr);
        out.data = slicer.GetCopyOfTile(view_, dc_.toTileDepth(view_));
        out.size = chunkSize_ * chunkSize_;
        return out;
    }
};

data_var operator>>(const dataSrcs& d, const sliceTile& v) {
    return boost::apply_visitor(v, d);
}

}}
