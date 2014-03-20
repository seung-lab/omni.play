#pragma once
#include "precomp.h"

/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

//#define DYNAMIC_FOREST_ASSERT(x) (assert(x))
#define DYNAMIC_FOREST_ASSERT(x) (static_cast<void>(sizeof(x)))

namespace zi {

template <typename T>
class DynamicForestPool {
 private:
  struct Node {
    T l_, r_, p_, pp_;

    Node() : l_(0), r_(0), p_(0), pp_(0) {}
  };

  Node* x_;
  size_t size_;

 public:
  DynamicForestPool(size_t s) : size_(s + 1) { init(); }

  ~DynamicForestPool() { free(x_); }

 private:
  uint64_t numBytes() const { return numBytes(size_); }

  static uint64_t numBytes(const size_t s) { return s * sizeof(Node); }

  void init() {
    // enough to set everything to zero
    // no need to call the constructors

    x_ = static_cast<Node*>(malloc(numBytes()));

    if (!x_) {
      throw std::bad_alloc();
    }

    clear();
  }

  void clear() { memset(x_, 0, numBytes()); }

  void resize(const size_t newSize) {
    if (newSize == size_) {
      return;
    }

    if (newSize <= size_) {
      throw om::ArgException(
          "we can only grow, shrinking doesn't make sense!!!");
    }

    resizeNumBytes(numBytes(size_), numBytes(newSize));

    size_ = newSize;
  }

  void resizeNumBytes(const size_t oldSizeBytes, const size_t newSizeBytes) {
    Node* ret = static_cast<Node*>(realloc(x_, newSizeBytes));

    if (!ret) {
      throw std::bad_alloc();
    }

    x_ = ret;

    memset(&x_[size_], 0, newSizeBytes - oldSizeBytes);
  }

  inline void left(const T n) {
    DYNAMIC_FOREST_ASSERT(x_[n].r_);

    const T x(x_[n].r_);
    const T z(x_[n].p_);

    if (z) {
      if (x_[z].l_ == n) {
        x_[z].l_ = x;

      } else {
        DYNAMIC_FOREST_ASSERT(x_[z].r_ == n);
        x_[z].r_ = x;
      }

    } else {
      std::swap(x_[x].pp_, x_[n].pp_);
    }

    x_[n].r_ = x_[x].l_;
    x_[x].l_ = n;

    x_[x].p_ = z;
    x_[n].p_ = x;

    if (x_[n].r_) {
      x_[x_[n].r_].p_ = n;
    }
  }

  inline void right(const T n) {
    DYNAMIC_FOREST_ASSERT(x_[n].l_);

    const T x(x_[n].l_);
    const T z(x_[n].p_);

    if (z) {
      if (x_[z].r_ == n) {
        x_[z].r_ = x;

      } else {
        DYNAMIC_FOREST_ASSERT(x_[z].l_ == n);
        x_[z].l_ = x;
      }

    } else {
      std::swap(x_[x].pp_, x_[n].pp_);
    }

    x_[n].l_ = x_[x].r_;
    x_[x].r_ = n;

    x_[x].p_ = z;
    x_[n].p_ = x;

    if (x_[n].l_) {
      x_[x_[n].l_].p_ = n;
    }
  }

  inline void splay(const T n) {
    while (x_[n].p_) {
      const T g(x_[x_[n].p_].p_);

      if (n == x_[x_[n].p_].l_) {
        if (g && (x_[n].p_ == x_[g].l_)) {
          right(g);
        }
        right(x_[n].p_);

      } else {
        DYNAMIC_FOREST_ASSERT(n == x_[x_[n].p_].r_);

        if (g && (x_[n].p_ == x_[g].r_)) {
          left(g);
        }
        left(x_[n].p_);
      }
    }
  }

  void access(const T n) {
    splay(n);

    if (x_[n].r_) {
      x_[x_[n].r_].pp_ = n;
      x_[x_[n].r_].p_ = 0;
      x_[n].r_ = 0;
    }

    T w;
    T v(n);

    while (x_[v].pp_) {
      w = x_[v].pp_;
      splay(w);

      if (x_[w].r_) {
        x_[x_[w].r_].pp_ = w;
        x_[x_[w].r_].p_ = 0;
      }

      x_[w].r_ = v;
      x_[v].p_ = w;
      x_[v].pp_ = 0;
      v = w;
    }

    splay(n);
  }

  void print(const int d, const int o) {
    if (x_[d].l_) {
      print(x_[d].l_, o + 2);
    }

    log_debugs << std::string(o, ' ') << d << " (" << x_[d].l_ << ", "
               << x_[d].r_ << ", " << x_[d].p_ << ", " << x_[d].pp_ << ")";

    if (x_[d].r_) {
      print(x_[d].r_, o + 2);
    }
  }

 public:
  void Print(const int d) {
    print(d + 1, 0);
    log_debugs << "---";
  }

  void Clear() { clear(); }

  size_t Size() { return size_ - 1; }

  void Resize(const size_t newSize) { resize(newSize + 1); }

  // 0 is not a valid n
  void Cut(const T n) {
    DYNAMIC_FOREST_ASSERT(n < size_ && n > 0);
    access(n + 1);

    if (x_[n + 1].l_) {
      x_[x_[n + 1].l_].p_ = 0;
      x_[n + 1].l_ = 0;
    }
  }

  void Join(const T n, const T w) {
    DYNAMIC_FOREST_ASSERT(n < size_ && n > 0);
    DYNAMIC_FOREST_ASSERT(w < size_ && w > 0);

    access(n + 1);
    DYNAMIC_FOREST_ASSERT(x_[n + 1].l_ == 0);
    access(w + 1);

    x_[n + 1].l_ = w + 1;
    x_[w + 1].p_ = n + 1;
  }

  T Root(const T n) {
    DYNAMIC_FOREST_ASSERT(n < size_ && n > 0);

    access(n + 1);

    T v = n + 1;

    while (x_[v].l_) {
      v = x_[v].l_;
    }

    splay(v);

    return v - 1;
  }
};

}  // namespace zi

#undef DYNAMIC_FOREST_ASSERT
