#pragma once

#include "pipeline/stage.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "datalayer/memMappedFile.hpp"

namespace om {
namespace pipeline {

class sliceTile : public stage
{
private:
    om::common::ViewType view_;
    coords::Data dc_;
    int chunkSize_; // TODO: rewrite for arbitrary chunks

public:
    sliceTile(om::common::ViewType view, coords::Data dc)
        : view_(view)
        , dc_(dc)
        , chunkSize_(dc.volume()->ChunkDimensions().x)
    { }

    template <typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        coords::Chunk cc = dc_.ToChunk();
        uint64_t offset = cc.PtrOffset(dc_.volume(), sizeof(T));
        T* chunkPtr = in.GetPtrWithOffset(offset);
        chunks::rawChunkSlicer<T> slicer(chunkSize_, chunkPtr);
        out.data = slicer.GetCopyOfTile(view_, dc_.ToTileDepth(view_));
        out.size = chunkSize_ * chunkSize_;
        return out;
    }
};

data_var operator>>(const dataSrcs& d, const sliceTile& v) {
    return boost::apply_visitor(v, d);
}

}}
