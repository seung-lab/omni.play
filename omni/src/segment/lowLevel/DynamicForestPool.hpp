/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _DYNAMIC_FOREST_POOL_HPP
#define _DYNAMIC_FOREST_POOL_HPP

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

namespace zi {

template<typename T>
class DynamicForestPool {
private:
  struct Node {
    T l_, r_, p_, pp_;
    Node(): l_(0), r_(0), p_(0), pp_(0) {}
  };
  Node   *x_;
  size_t size_;

public:

  DynamicForestPool(size_t s): size_(s) {
    // enough to set everything to zero
    // no need to call the constructors
    x_ = (Node*)malloc(s + s * sizeof(Node));
    memset((char*)x_, 0, s + s * sizeof(Node));
  }
  ~DynamicForestPool() {
    free(x_);
  }

private:

  inline void left_(T n) {
    assert(x_[n].r_);
    T x(x_[n].r_), z(x_[n].p_);

    if (z) {
      if (x_[z].l_ == n) {
        x_[z].l_ = x;
      } else {
        assert(x_[z].r_ == n);
        x_[z].r_ = x;
      }
    } else {
      std::swap(x_[x].pp_, x_[n].pp_);
    }

    x_[n].r_  = x_[x].l_;  x_[x].l_  = n;
    x_[x].p_  = z;         x_[n].p_  = x;

    if (x_[n].r_) x_[x_[n].r_].p_ = n;
  }

  inline void right_(T n) {
    assert(x_[n].l_);
    T x(x_[n].l_), z(x_[n].p_);

    if (z) {
      if (x_[z].r_ == n) {
        x_[z].r_ = x;
      } else {
        assert(x_[z].l_ == n);
        x_[z].l_ = x;
      }
    } else {
      std::swap(x_[x].pp_, x_[n].pp_);
    }

    x_[n].l_  = x_[x].r_;  x_[x].r_  = n;
    x_[x].p_  = z;         x_[n].p_  = x;

    if (x_[n].l_) x_[x_[n].l_].p_ = n;
  }

  inline void splay_(T n) {
    while (x_[n].p_) {
      T g(x_[x_[n].p_].p_);
      if (n == x_[x_[n].p_].l_) {
        if (g && (x_[n].p_ == x_[g].l_)) right_(g);
        right_(x_[n].p_);
      } else {
        assert (n == x_[x_[n].p_].r_);
        if (g && (x_[n].p_ == x_[g].r_)) left_(g);
        left_(x_[n].p_);
      }
    }
  }

  void access_(T n) {
    splay_(n);
    if (x_[n].r_) {
      x_[x_[n].r_].pp_ = n;
      x_[x_[n].r_].p_  = 0;
      x_[n].r_         = 0;
    }
    T w, v(n);
    while (x_[v].pp_) {
      w = x_[v].pp_;
      splay_(w);
      if (x_[w].r_) {
        x_[x_[w].r_].pp_ = w;
        x_[x_[w].r_].p_  = 0;
      }
      x_[w].r_  = v;
      x_[v].p_  = w;
      x_[v].pp_ = 0;
      v = w;
    }
    splay_(n);
  }

  void print_(int d, int o) {
    if (x_[d].l_) print_(x_[d].l_, o + 2);
    std::cout << std::string(o, ' ') << d << " ("
              << x_[d].l_ << ", "
              << x_[d].r_ << ", "
              << x_[d].p_ << ", "
              << x_[d].pp_
              << ")" << std::endl;
    if (x_[d].r_) print_(x_[d].r_, o+2);
  }

public:

  void print(int d) {
    print_(d+1,0);
    std::cout << "---\n";
  }

  void clear() {
    memset((char*)x_, 0, size_ + size_ * sizeof(Node));
  }

  size_t size() {
    return size_;
  }

  // we can only grow, shrinking doesn't make sense!!!
  void resize(size_t s) {
    assert(s > size_);
    x_ = realloc(x_, s + s * sizeof(Node));
    memset((char*)(&x_[size_+1]), 0, s - size_);
    size_ = s;
  }

  void cut(T n) {
    assert(n < size_ && n >= 0);
    access_(n+1);
    if (x_[n+1].l_) {
      x_[x_[n+1].l_].p_ = 0;
      x_[n+1].l_        = 0;
    }

  }

  void join(T n, T w) {
    assert(n < size_ && n >= 0);
    assert(w < size_ && w >= 0);

    access_(n+1);
    assert(x_[n+1].l_ == 0);
    access_(w+1);

    x_[n+1].l_ = w+1;
    x_[w+1].p_ = n+1;
  }

  T root(T n) {
    assert(n < size_ && n >= 0);

    access_(n+1);
    T v = n+1;
    while (x_[v].l_)
      v = x_[v].l_;
    splay_(v);
    return v-1;
  }

};

}

#endif
