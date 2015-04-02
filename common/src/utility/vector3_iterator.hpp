#pragma once

#include "precomp.h"
#include "common/common.h"

namespace om {
namespace utility {

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
      : from_(from),
        to_(to),
        val_(curr),
        valid_(from.x <= curr.x && from.y <= curr.y && from.z <= curr.z &&
               curr.x <= to.x && curr.y <= to.y && curr.z <= to.z) {
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
}
}  // namespace om::utility::