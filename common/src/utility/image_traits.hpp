#pragma once

#include <boost/multi_array.hpp>

namespace om {
namespace utility {

template <int I, int D> struct MakeBoostRange;

template <int D> struct dimension {
 public:
  boost::detail::multi_array::extent_gen<D> getBoostExtent() {
    return subDimension_.getBoostExtent()[dimValue_];
  }

  dimension() : subDimension_(dimension<D - 1>()), dimValue_(0) {}

  dimension(dimension<D - 1> subDim, int value)
      : subDimension_(subDim), dimValue_(value) {}

  dimension<D + 1> operator[](int idx) const {
    return dimension<D + 1>(*this, idx);
  }

  template <int I> inline int get() {
    assert(I < D && I >= 0);
    if (I == D - 1) return dimValue_;
    return subDimension_.get<I>();
  }

  template <int I> inline void set(int x) {
    assert(I < D && I >= 0);
    if (I == D - 1) {
      dimValue_ = x;
    } else {
      subDimension_.set<I>(x);
    }
  }

  template <int I> dimension<D - 1> stripDimension() {
    assert(I < D && I >= 0);
    if (I == D - 1) {
      return subDimension_;
    } else {
      return subDimension_.stripDimension<I>()[dimValue_];
    }
  }

  template <int I, int X> friend class MakeBoostRange;

  static dimension ONE;
  static dimension ZERO;

 private:
  dimension<D - 1> subDimension_;
  int dimValue_;
};

template <int D> dimension<D> dimension<D>::ONE = dimension<D - 1>::ONE[1];

template <int D> dimension<D> dimension<D>::ZERO = dimension<D - 1>::ZERO[0];

template <> struct dimension<0> {
 public:
  dimension() {}
  boost::detail::multi_array::extent_gen<0> getBoostExtent() {
    return boost::extents;
  }

  dimension(dimension<1>) { assert(0); }

  dimension<1> operator[](int idx) const { return dimension<1>(*this, idx); }

  template <int I> inline int get() {
    assert(false);
    return 0;
  }

  template <int I> inline void set(int) { assert(false); }

  template <int I> dimension<0> stripDimension() {
    assert(false);
    return dimension<0>();
  }

  static const dimension<0> extent();

  static const dimension<0> ONE();
  static const dimension<0> ZERO();

};

//namespace {
//template<> dimension<0> dimension<0>::ONE  = dimension<0>();
//template<> dimension<0> dimension<0>::ZERO = dimension<0>();
//}

template <int I, int D> struct MakeBoostRange {
 public:
  static boost::detail::multi_array::index_gen<I, D> make(dimension<I> from,
                                                          dimension<I> to) {
    if (from.dimValue_ == to.dimValue_) {
      boost::detail::multi_array::index_gen<I - 1, D> range =
          MakeBoostRange<I - 1, D>::make(from.subDimension_, to.subDimension_);
      return range[from.dimValue_];
    }

    boost::detail::multi_array::index_gen<I - 1, D - 1> range =
        MakeBoostRange<I - 1, D - 1>::make(from.subDimension_,
                                           to.subDimension_);

    return range[
        boost::multi_array_types::index_range(from.dimValue_, to.dimValue_)];
  }

  static boost::detail::multi_array::index_gen<I, D> makeLen(dimension<I> from,
                                                             dimension<I> len) {
    boost::detail::multi_array::index_gen<I - 1, D - 1> range =
        MakeBoostRange<I - 1, D - 1>::makeLen(from.subDimension_,
                                              len.subDimension_);

    return range[boost::multi_array_types::index_range(
        from.dimValue_, from.dimValue_ + len.dimValue_)];
  }

};

template <int D> struct MakeBoostRange<0, D> {
 public:
  static boost::detail::multi_array::index_gen<0, D> make(dimension<0>,
                                                          dimension<0>) {
    return boost::detail::multi_array::index_gen<0, D>();
  }

  static boost::detail::multi_array::index_gen<0, D> makeLen(dimension<0>,
                                                             dimension<0>) {
    return boost::detail::multi_array::index_gen<0, D>();
  }

};

static dimension<0> extents;

}
}  // namespace om::utility