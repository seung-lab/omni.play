#pragma once

#include "chunks/chunk.hpp"
#include "pipeline/stage.hpp"

namespace om {
namespace tiles {

class Tile
{
private:
    const coords::volumeSystem* vol_;
    const chunks::ChunkVar& chunk_;
    const coords::data& dc_;
    const common::viewType& view_;

public:
    Tile(const coords::volumeSystem* vol,
         const chunks::ChunkVar& chunk,
         const coords::data& dc,
         const common::viewType& view)
        : vol_(vol)
        , chunk_(chunk)
        , dc_(dc)
        , view_(view)
    {}

    void WriteJpeg(server::tile& t);
    void WritePng(server::tile& t);
    void WriteBitmaskedPng(int32_t segId, server::tile& t);

private:
    void write(server::tile& t, const pipeline::data_var& encoded);
};

}} // namespace om::tiles::
