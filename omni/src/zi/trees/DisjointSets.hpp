#ifndef _ZI_TREES_DISJOINT_SETS_HPP_
#define _ZI_TREES_DISJOINT_SETS_HPP_

#include <cstring>
#include <cassert>
#include <cstdlib>

#include <iostream>

namespace zi {

template<typename T>
class DisjointSets {

private:
  struct Node { T r, p; Node(): r(0), p(0) {} };
  Node  *x_;
  size_t size_;
  size_t nSets_;

public:

  DisjointSets(size_t s) : size_(s), nSets_(s) {
    assert(s >= 0);
    x_ = new Node[s];
    for (int i=0; i<s; ++i) x_[i].p = i;
  }

  inline T findSet(T id) const {
    assert(id < size_);
    T n(id), x;
    while (n != x_[n].p) n = x_[n].p;
    while (n != id) { x = x_[id].p; x_[id].p = n; id = x; }
    return n;
  }

  inline T join(T x, T y) {
    assert(x < size_);
    assert(y < size_);

    if (x == y) return x;
    --nSets_;

    if (x_[x].r >= x_[y].r) {
      x_[y].p = x;
      if (x_[x].r == x_[y].r) ++x_[x].r;
      return x;
    }

    x_[x].p = y;
    return y;
  }

  inline void clear() {
    for (int i=0; i<size_; ++i) { x_[i].p = i; x_[i].r = 0; }
    nSets_ = size_;
  }

  size_t size()     const { return size_ ; }
  size_t noOfSets() const { return nSets_; }
  ~DisjointSets()         { free(x_);      }

};

}

#endif
