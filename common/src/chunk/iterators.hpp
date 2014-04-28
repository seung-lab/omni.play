#pragma once

#include "utility/vector3_iterator.hpp"
#include "chunk/dataSources.hpp"
#include "coordinates/chunk.h"

namespace om {
namespace chunk {
// Iterates over chunks in a given range at a given mip level.  Built using the
// vector_iterator.
typedef boost::transform_iterator<std::function<coords::Chunk(const Vector3i&)>,
                                  utility::vector3_iterator<int>> iterator;

inline iterator make_iterator(int mipLevel,
                              utility::vector3_iterator<int> iter) {
  return iterator(iter, [mipLevel](const Vector3i& vec) {
    return coords::Chunk(mipLevel, vec);
  });
}

inline iterator make_iterator(const coords::Chunk& from,
                              const coords::Chunk& to) {
  return make_iterator(from.mipLevel(),
                       utility::vector3_iterator<int>(from, to));
}

// Iterates over specific chunks in a given range at a given mip level. Takes a
// filter function to decide what chunks to use. Built using the iterator.
typedef boost::filter_iterator<std::function<bool(const coords::Chunk&)>,
                               iterator> filtered_iterator;

inline filtered_iterator make_filtered_iterator(
    std::function<bool(const coords::Chunk&)> filter, iterator iter) {
  return filtered_iterator(filter, iter);
}

inline filtered_iterator make_filtered_iterator(
    std::function<bool(const coords::Chunk&)> filter, const coords::Chunk& from,
    const coords::Chunk& to) {
  return make_filtered_iterator(filter, make_iterator(from, to));
}

// Iterates over specific chunks in a given range at a given mip level.  This
// only yeilds chunks which contain segments in the given set.  Built using the
// filtered_iterator.
inline filtered_iterator make_segment_iterator(chunk::UniqueValuesDS& uvm,
                                               common::SegID segment,
                                               iterator iter) {
  return make_filtered_iterator([&uvm, segment](const coords::Chunk& c) {
                                  auto uv = uvm.Get(c);
                                  return !uv || uv->contains(segment);
                                },
                                iter);
}

inline filtered_iterator make_segment_iterator(const coords::Chunk& from,
                                               const coords::Chunk& to,
                                               chunk::UniqueValuesDS& uvm,
                                               common::SegID segment) {
  return make_segment_iterator(uvm, segment, make_iterator(from, to));
}

// Iterates over specific chunks in a given range at a given mip level.  This
// only yeilds chunks which contain segments in the given set.  Built using the
// filtered_iterator.
inline filtered_iterator make_segset_iterator(chunk::UniqueValuesDS& uvm,
                                              common::SegIDSet segs,
                                              iterator iter) {
  return make_filtered_iterator([&uvm, segs](const coords::Chunk& c) {
                                  auto uv = uvm.Get(c);
                                  return !uv || uv->contains_any(segs.begin(),
                                                                 segs.end());
                                },
                                iter);
}

inline filtered_iterator make_segset_iterator(const coords::Chunk& from,
                                              const coords::Chunk& to,
                                              chunk::UniqueValuesDS& uvm,
                                              common::SegIDSet segs) {
  return make_segset_iterator(uvm, segs, make_iterator(from, to));
}
}
}  // namespace om::chunk::
