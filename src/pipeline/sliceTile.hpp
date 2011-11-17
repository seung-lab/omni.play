#pragma once

#include "pipeline/stage.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "datalayer/fs/memMappedFile.hpp"

namespace om {
namespace pipeline {

class sliceTile : public stage
{
private:
    common::viewType view_;
    coords::dataCoord dc_;
    int chunkSize_;

public:
    sliceTile(common::viewType view, coords::dataCoord dc)
        : view_(view)
        , dc_(dc)
        , chunkSize_(dc.volume()->GetChunkDimension())
    { }

    template <typename T>
    data_var operator()(const datalayer::memMappedFile<T>& in) const
    {
        data<T> out;
        std::cout << "Executing sliceTile stage." << std::endl;
        T* chunkPtr = in.GetPtr() + dc_.toChunkCoord().chunkPtrOffset(dc_.volume(), sizeof(T));
        chunks::rawChunkSlicer<T> slicer(chunkSize_, chunkPtr);
        out.data = slicer.GetCopyOfTile(view_, dc_.toTileDepth(view_));
        out.size = chunkSize_ * chunkSize_;
        return out;
    }
};

data_var operator>>(const dataSrcs& d, const sliceTile& v) {
    return boost::apply_visitor(v, d);
}

}
}
