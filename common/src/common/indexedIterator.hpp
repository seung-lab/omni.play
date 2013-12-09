#pragma once

#include <boost/iterator/iterator_facade.hpp>

// TODO: Conversions between const and non-const iterators
template <typename TColl, typename TData>
class indexed_iterator : public boost::iterator_facade<
    indexed_iterator<TColl, TData>, TData, std::random_access_iterator_tag> {
 public:
  indexed_iterator(TColl& coll, size_t idx) : coll_(&coll), idx_(idx) {}

 private:
  friend class boost::iterator_core_access;

  TData& dereference() const { return (*coll_)[idx_]; }

  bool equal(const indexed_iterator& y) const {
    return coll_ == y.coll_ && idx_ == y.idx_;
  }

  void increment() { idx_++; }

  void decrement() { idx_--; }

  void advance(size_t n) { idx_ += n; }

  size_t distance_to(const indexed_iterator& z) const { return z.idx_ - idx_; }

  TColl* coll_;
  size_t idx_;
};