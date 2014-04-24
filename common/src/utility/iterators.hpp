#pragma once

#include "precomp.h"
#include "common/common.h"
#include "coordinates/chunk.h"
#include "chunk/voxelGetter.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace utility {

template <class T, class U>
void carry(T& a, const T& a_from, const T& a_to, U& b) {
  const T diff = a_to - a_from + 1;  // inclusive
  if (a < a_from) {
    auto dist = (a_from - a);
    auto carries = dist / diff;
    if (dist % diff) {
      carries++;
    }
    a += carries * diff;
    b -= carries;
  }
  if (a > a_to) {
    auto dist = (a - a_to);
    auto carries = dist / diff;
    if (dist % diff) {
      carries++;
    }
    a -= carries * diff;
    b += carries;
  }
}

template <class T, class U, class... TRest>
void carry(T& a, const T& a_from, const T& a_to, U& b, TRest&... rest) {
  carry(a, a_from, a_to, b);
  carry(b, rest...);
}

template <class T>
inline void increment(T& a) {
  a++;
}

template <class T, class U, class... TRest>
inline void increment(T& a, const T& a_from, const T& a_to, U& b,
                      TRest&... rest) {
  if (++a > a_to) {
    a = a_from;
    increment(b, rest...);
  }
}

template <class T>
inline void decrement(T& a) {
  a++;
}

template <class T, class U, class... TRest>
inline void decrement(T& a, const T& a_from, const T& a_to, U& b,
                      TRest&... rest) {
  if (++a > a_to) {
    a = a_from;
    decrement(b, rest...);
  }
}

// Iterates through a 3d range of vector values [from, to] inclusive.
template <typename T>
class vector3_iterator
    : public boost::iterator_facade<vector3_iterator<T>, const Vector3<T>,
                                    std::bidirectional_iterator_tag> {
 public:
  vector3_iterator()
      : from_(Vector3<T>::ZERO),
        to_(Vector3<T>::ZERO),
        val_(Vector3<T>::ZERO),
        valid_(false) {}
  vector3_iterator(Vector3<T> from, Vector3<T> to, Vector3<T> curr)
      : from_(from), to_(to), val_(curr), valid_(from <= curr && curr <= to) {
    if (!valid_) {
      log_errors << "Invalid iterator: " << from << "<=" << curr << "<=" << to;
    }
  }
  vector3_iterator(Vector3<T> from, Vector3<T> to)
      : vector3_iterator(from, to, from) {}
  vector3_iterator(const vector3_iterator& other)
      : from_(other.from_),
        to_(other.to_),
        val_(other.val_),
        valid_(other.valid_) {}

 private:
  friend class boost::iterator_core_access;

  const Vector3<T>& dereference() const { return val_; }

  bool equal(const vector3_iterator& y) const {
    return (!y.valid_ && !valid_) || (y.valid_ && val_ == y.val_);
  }

  void increment() {
    // utility::increment(val_.z, from_.z, to_.z, val_.y, from_.y, to_.y,
    // val_.x);
    if (++val_.z > to_.z) {
      val_.z = from_.z;
      if (++val_.y > to_.y) {
        val_.y = from_.y;
        if (++val_.x > to_.x) {
          val_.x = from_.x;
          valid_ = false;
        }
      }
    }
  }

  void decrement() {
    // utility::decrement(val_.z, from_.z, to_.z, val_.y, from_.y, to_.y,
    // val_.x);
    if (--val_.z < from_.z) {
      val_.z = to_.z;
      if (--val_.y < from_.y) {
        val_.y = to_.y;
        if (--val_.x < from_.x) {
          val_.x = to_.x;
          valid_ = false;
        }
      }
    }
  }

  Vector3<T> from_, to_;
  Vector3<T> val_;
  bool valid_;
};

// Iterates over chunks in a given range at a given mip level.  Built using the
// vector_iterator.
typedef boost::transform_iterator<std::function<coords::Chunk(const Vector3i&)>,
                                  vector3_iterator<int>> chunk_iterator;

inline chunk_iterator make_chunk_iterator(int mipLevel,
                                          vector3_iterator<int> iter) {
  return chunk_iterator(iter, [mipLevel](const Vector3i& vec) {
    return coords::Chunk(mipLevel, vec);
  });
}

inline chunk_iterator make_chunk_iterator(const coords::Chunk& from,
                                          const coords::Chunk& to) {
  return make_chunk_iterator(from.mipLevel(), vector3_iterator<int>(from, to));
}

// // Iterates over specific chunks in a given range at a given mip level.
// Takes a
// // filter function to decide what chunks to use. Built using the
// chunk_iterator.
// typedef boost::filter_iterator<std::function<bool(const coords::Chunk&)>,
//                                chunk_iterator> filtered_chunk_iterator;

// inline filtered_chunk_iterator make_filtered_chunk_iterator(
//     std::function<bool(const coords::Chunk&)> filter, chunk_iterator iter) {
//   return filtered_chunk_iterator(filter, iter);
// }

// inline filtered_chunk_iterator make_filtered_chunk_iterator(
//     std::function<bool(const coords::Chunk&)> filter, const coords::Chunk&
// from,
//     const coords::Chunk& to) {
//   return make_filtered_chunk_iterator(filter, make_chunk_iterator(from, to));
// }

// // Iterates over specific chunks in a given range at a given mip level.  This
// // only yeilds chunks which contain segments in the given set.  Built using
// the
// // filtered_chunk_iterator.
// inline filtered_chunk_iterator make_segset_chunk_iterator(
//     chunk::UniqueValuesDS& uvm, common::SegIDSet segs, chunk_iterator iter) {
//   return make_filtered_chunk_iterator([&uvm, segs](const coords::Chunk& c) {
//                                         auto uv = uvm.Get(c);
//                                         if (!uv) {
//                                           return false;
//                                         }
//                                         return std::find_if(
//                                                    segs.begin(), segs.end(),
//                                                    [&uv](common::SegID s) {
//                                                      return uv->contains(s);
//                                                    }) != segs.end();
//                                       },
//                                       iter);
// }

// inline filtered_chunk_iterator make_segset_chunk_iterator(
//     chunk::UniqueValuesDS& uvm, common::SegIDSet segs,
//     const coords::Chunk& from, const coords::Chunk& to) {
//   return make_segset_chunk_iterator(uvm, segs, make_chunk_iterator(from,
// to));
// }

// // Iterates over coords::Data in a given range at a given mip level.  Built
// // using the vector_iterator.
// typedef boost::transform_iterator<std::function<coords::Data(const
// Vector3i&)>,
//                                   vector3_iterator<int>> data_iterator;

// inline data_iterator make_data_iterator(const coords::VolumeSystem& sys,
//                                         int mipLevel,
//                                         vector3_iterator<int> iter) {
//   return data_iterator(iter, [&sys, mipLevel](const Vector3i& vec) {
//     return coords::Data(vec, sys, mipLevel);
//   });
// }

// inline data_iterator make_data_iterator(const coords::Data& from,
//                                         const coords::Data& to) {
//   return make_data_iterator(from.volume(), from.mipLevel(),
//                             vector3_iterator<int>(from, to));
// }

// // Iterates over chunks in a given range at a given mip level.  Built using
// the
// // vector_iterator.
// template <typename T>
// using dataval_iterator = boost::transform_iterator<
//     std::function<std::pair<coords::Data, T>(const coords::Data&)>,
//     data_iterator>;

// template <typename T>
// inline dataval_iterator<T> make_dataval_iterator(chunk::Voxels<T>& voxels,
//                                                  data_iterator iter) {
//   return dataval_iterator<T>(iter, [&voxels](const coords::Data& d) {
//     return std::make_pair(d, voxels.GetValue(d));
//   });
// }

// template <typename T>
// inline dataval_iterator<T> make_dataval_iterator(chunk::Voxels<T>& voxels,
//                                                  const coords::Data& from,
//                                                  const coords::Data& to) {
//   return make_dataval_iterator<T>(voxels, make_data_iterator(from, to));
// }

// // Iterates over specific chunks in a given range at a given mip level.
// Takes a
// // filter function to decide what chunks to use. Built using the
// chunk_iterator.
// template <typename T>
// using filtered_dataval_iterator = boost::filter_iterator<
//     std::function<bool(const std::pair<coords::Data, T>&)>,
//     dataval_iterator<T>>;

// template <typename T>
// filtered_dataval_iterator<T> make_filtered_dataval_iterator(
//     std::function<bool(const std::pair<coords::Data, T>&)> filter,
//     chunk::Voxels<T>& voxels, const coords::Data& from,
//     const coords::Data& to) {
//   return filtered_dataval_iterator<T>(filter,
//                                       make_dataval_iterator(voxels, from,
// to));
// }

// template <typename T>
// class chunkfiltered_dataval_iterator
//     : public boost::iterator_adaptor<
//           chunkfiltered_dataval_iterator<T>, filtered_chunk_iterator,
//           std::pair<coords::Data, T>, std::forward_iterator_tag,
//           const std::pair<coords::Data, T>&> {
//  public:
//   chunkfiltered_dataval_iterator()
//       : chunkfiltered_dataval_iterator<T>::iterator_adaptor_(),
//         voxels_(nullptr) {}
//   chunkfiltered_dataval_iterator(
//       coords::Data from, coords::Data to, coords::Data curr,
//       std::function<bool(const coords::Chunk&)> filter,
//       chunk::Voxels<T>& voxels)
//       : chunkfiltered_dataval_iterator<T>::iterator_adaptor_(
//             make_filtered_chunk_iterator(
//                 filter,
//                 make_chunk_iterator(from.mipLevel(),
//                                     vector3_iterator<int>(from.ToChunk(),
//                                                           to.ToChunk(),
//                                                           curr.ToChunk())))),
//         from_(new coords::Data(from)),
//         to_(new coords::Data(to)),
//         voxels_(&voxels) {
//     if (from.mipLevel() != to.mipLevel() ||
//         from.mipLevel() != curr.mipLevel()) {
//       throw ArgException(
//           "Data Coords must have the same mip level for iterator.");
//     }
//     val_.reset(std::make_pair(curr, voxels_->GetValue(curr)));
//     updateChunkRange();
//     carry();
//   }

//   chunkfiltered_dataval_iterator(const chunkfiltered_dataval_iterator& other)
//       : chunkfiltered_dataval_iterator<T>::iterator_adaptor_(other),
//         from_(new coords::Data(*other.from_)),
//         to_(new coords::Data(*other.to_)),
//         chunkFrom_(other.chunkFrom_),
//         chunkTo_(other.chunkTo_),
//         voxels_(other.voxels_),
//         val_(other.val_) {}

//  private:
//   friend class boost::iterator_core_access;

//   const std::pair<coords::Data, common::SegID>& dereference() const {
//     if (val_) {
//       return val_.get();
//     } else {
//       throw InvalidOperationException(
//           "Dereferencing Invalid chunkfiltered_dataval_iterator Iterator.");
//     }
//   }

//   bool equal(const chunkfiltered_dataval_iterator& y) const {
//     if (!val_) {
//       return !y.val_;
//     }
//     return std::tie(*from_, *to_, voxels_, val_) ==
//            std::tie(*y.from_, *y.to_, y.voxels_, y.val_);
//   }

//   void increment() {
//     if (val_) {
//       val_.get().first.z += 1;
//       carry();
//     } else {
//       throw InvalidOperationException(
//           "Incrementing Invalid chunkfiltered_dataval_iterator Iterator.");
//     }
//   }

//   void decrement() {
//     if (val_) {
//       val_.get().first.z -= 1;
//       carry();
//     } else {
//       throw InvalidOperationException(
//           "Decrementing Invalid chunkfiltered_dataval_iterator Iterator.");
//     }
//   }

//   void carry() {
//     if (val_) {
//       auto& ref = val_.get().first;
//       utility::carry(ref.z, chunkFrom_.z, chunkTo_.z, ref.y, chunkFrom_.y,
//                      chunkTo_.y, ref.x);
//       if (ref > chunkTo_) {
//         if (++this->base_reference() ==
//             typename chunkfiltered_dataval_iterator<T>::base_type()) {
//           val_.reset();
//           return;
//         } else {
//           updateChunkRange();
//           ref = coords::Data(chunkFrom_, ref.volume(), ref.mipLevel());
//         }
//       }
//       val_.get().second = voxels_->GetValue(ref);
//     }
//   }

//   void updateChunkRange() {
//     auto bounds = this->base()->BoundingBox(from_->volume());
//     chunkFrom_ = std::max(bounds.getMin(), *from_);
//     chunkTo_ = std::min(bounds.getMax(), *to_);
//   }

//   std::unique_ptr<coords::Data> from_, to_;
//   Vector3i chunkFrom_, chunkTo_;

//   chunk::Voxels<T>* const voxels_;
//   mutable boost::optional<std::pair<coords::Data, T>> val_;
// };

// inline chunkfiltered_dataval_iterator<common::SegID>
// make_segset_chunkfiltered_dataval_iterator(coords::Data from, coords::Data
// to,
//                                            coords::Data curr,
//                                            chunk::Voxels<common::SegID>&
// voxels,
//                                            chunk::UniqueValuesDS& uvm,
//                                            common::SegIDSet vals) {
//   return chunkfiltered_dataval_iterator<common::SegID>(
//       from, to, curr,
//       [&uvm, vals](const coords::Chunk& c) {
//         auto uv = uvm.Get(c);
//         if (!uv) {
//           return false;
//         }
//         return std::find_if(vals.begin(), vals.end(), [&uv](common::SegID s)
// {
//                  return uv->contains(s);
//                }) != vals.end();
//       },
//       voxels);
// }

// // Iterates over specific chunks in a given range at a given mip level.
// Takes a
// // filter function to decide what chunks to use. Built using the
// chunk_iterator.
// typedef boost::filter_iterator<
//     std::function<bool(const std::pair<coords::Data, common::SegID>&)>,
//     chunkfiltered_dataval_iterator<common::SegID>> segset_iterator;

// inline segset_iterator make_segset_iterator(
//     coords::Data from, coords::Data to, coords::Data curr,
//     chunk::Voxels<common::SegID>& voxels, chunk::UniqueValuesDS& uvm,
//     common::SegIDSet segs) {
//   return segset_iterator([segs](
//                              const std::pair<coords::Data, common::SegID>& c)
// {
//                            return segs.count(c.second);
//                          },
//                          make_segset_chunkfiltered_dataval_iterator(
//                              from, to, curr, voxels, uvm, segs));
// }
}
}  // namespace om::utility::