#include "tiles/tile.h"

#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"

namespace om {
using namespace pipeline;

namespace tiles {

void Tile::WriteJpeg(server::tile& t)
{
    data_var encoded = chunk_ >> sliceTile(view_, dc_) >> jpeg(128,128) >> encode();

    write(t, encoded);
}

void Tile::WritePng(server::tile& t)
{
    data_var encoded = chunk_ >> sliceTile(view_, dc_) >> png(128,128) >> encode();

    write(t, encoded);
}

void Tile::WriteBitmaskedPng(int32_t segId, server::tile& t)
{
    data_var encoded = chunk_ >> sliceTile(view_, dc_)
                              >> bitmask(segId)
                              >> png(128,128)
                              >> encode();

    write(t, encoded);
}

void Tile::write(server::tile& t, const data_var& encoded)
{
    coords::chunk cc = dc_.toChunk();
    int depth = dc_.toTileDepth(view_);
    coords::dataBbox bounds = cc.chunkBoundingBox(vol_);

    server::vector3d min = common::twist(bounds.getMin().toGlobal(), view_);
    server::vector3d max = common::twist(bounds.getMax().toGlobal(), view_);

    min.z += depth;
    max.z = min.z;

    t.bounds.min = common::twist(min, view_);
    t.bounds.max = common::twist(max, view_);
    t.view = common::Convert(view_);

    data<char> out = boost::get<data<char> >(encoded);
    t.data = std::string(out.data.get(), out.size);
}

}
}
