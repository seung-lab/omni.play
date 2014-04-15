#pragma once

#include "precomp.h"
#include "common/common.h"
#include "coordinates/chunk.h"

namespace om {
namespace utility {

template <class T, class U>
void carry(T& a, const T& a_from, const T& a_to, U& b) {
  const T diff = a_to - a_from + 1;  // inclusive
  while (a < a_from) {
    a += diff;
    b--;
  }
  while (a > a_to) {
    a -= diff;
    b++;
  }
}

template <class T, class U, class... TRest>
void carry(T& a, const T& a_from, const T& a_to, U& b, TRest&... rest) {
  carry(a, a_from, a_to, b);
  carry(b, rest...);
}

template <typename T>
class vector3_iterator
    : public boost::iterator_facade<vector3_iterator<T>, const Vector3<T>,
                                    std::bidirectional_iterator_tag> {
 public:
  vector3_iterator() : val_() {}
  vector3_iterator(Vector3<T> from, Vector3<T> to, Vector3<T> curr) {
    if (from < to) {
      from_ = from;
      to_ = to;
    } else {
      from_ = to;
      to_ = from;
    }
    if (from <= curr && curr <= to) {
      val_ = curr;
    }
    carry();
  }
  vector3_iterator(Vector3<T> from, Vector3<T> to)
      : vector3_iterator(from, to, from) {}

 private:
  friend class boost::iterator_core_access;

  const Vector3<T>& dereference() const {
    if (val_) {
      return val_.get();
    } else {
      throw InvalidOperationException("Dereferencing Invalid Vector Iterator.");
    }
  }

  bool equal(const vector3_iterator& y) const {
    if (!val_) {
      return !y.val_;
    }
    return std::tie(from_, to_, val_) == std::tie(y.from_, y.to_, y.val_);
  }

  void increment() {
    if (val_) {
      val_.get().z += 1;
      carry();
    } else {
      throw InvalidOperationException("Incrementing Invalid Vector Iterator.");
    }
  }

  void decrement() {
    if (val_) {
      val_.get().z -= 1;
      carry();
    } else {
      throw InvalidOperationException("Decrementing Invalid Vector Iterator.");
    }
  }

  void carry() {
    if (val_) {
      auto& ref = val_.get();
      utility::carry(ref.z, from_.z, to_.z, ref.y, from_.y, to_.y, ref.x);
      if (ref < from_ || to_ < ref) {
        val_.reset();
        if (val_) {
          std::cout << "WTF?" << std::endl;
        }
      }
    }
  }

  Vector3<T> from_, to_;
  mutable boost::optional<Vector3<T>> val_;
};

typedef boost::transform_iterator<std::function<coords::Chunk(const Vector3i&)>,
                                  vector3_iterator<int>> chunk_iterator;

chunk_iterator make_chunk_iterator(int mipLevel, Vector3i from, Vector3i to,
                                   Vector3i curr) {
  return chunk_iterator(
      vector3_iterator<int>(from, to, curr),
      [mipLevel](const Vector3i& vec) { return coords::Chunk(mipLevel, vec); });
}

chunk_iterator make_chunk_iterator(int mipLevel, Vector3i from, Vector3i to) {
  return make_chunk_iterator(mipLevel, from, to, from);
}

chunk_iterator make_chunk_iterator() {
  return chunk_iterator(vector3_iterator<int>(), [](const Vector3i& vec) {
    return coords::Chunk(0, vec);
  });
}
}
}  // namespace om::utility::