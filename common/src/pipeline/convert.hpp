#pragma once

#include "pipeline/stage.hpp"
#include "chunk/chunk.hpp"
#include "tile/tile.hpp"

namespace om {
namespace pipeline {

template <typename T> class Convert : public boost::static_visitor<Data<T>> {
 public:
  template <typename TIn>
  Data<T> operator()(const chunk::Chunk<TIn>& chunk) const {
    Data<T> out(chunk.length());
    std::copy(&chunk[0], &chunk[chunk.length()], out.data.get());
    return out;
  }

  template <typename TIn>
  Data<T> operator()(const tile::Tile<TIn>& tile) const {
    Data<T> out(tile.length());
    std::copy(&tile[0], &tile[tile.length()], out.data.get());
    return out;
  }
};

template <typename T>
    Data<T> operator>>(const chunk::ChunkVar& d, const Convert<T>& v) {
  return boost::apply_visitor(v, d);
}

template <typename T>
    Data<T> operator>>(const tile::TileVar& d, const Convert<T>& v) {
  return boost::apply_visitor(v, d);
}
}
}  // namespace om::pipeline::
