#pragma once

#include <boost/multi_array.hpp>

template <int I, int D> struct MakeBoostRange;

template <int D> struct OmDimension {
public:
	boost::detail::multi_array::extent_gen<D> getBoostExtent() {
		return subDimension_.getBoostExtent()[dimValue_];
	}

	OmDimension()
	: subDimension_(OmDimension<D-1>())
	, dimValue_(0)
	{}

	OmDimension(OmDimension<D-1> subDim, int value)
	: subDimension_(subDim)
	, dimValue_(value)
	{}

	OmDimension<D+1> operator[](int idx) const {
		return OmDimension<D+1>(*this, idx);
	}

  	template <int I>
	inline int get() {
		assert(I < D && I >= 0);
		if (I == D-1) return dimValue_;
		return subDimension_.get<I>();
	}

  	template <int I>
	inline void set(int x) {
		assert(I < D && I >= 0);
		if (I == D-1) {
			dimValue_ = x;
		} else {
			subDimension_.set<I>(x);
		}
	}

  	template <int I> OmDimension<D-1> stripDimension() {
	assert(I < D && I >= 0);
	if (I == D-1) {
		return subDimension_;
	} else {
		return subDimension_.stripDimension<I>()[dimValue_];
	}
}

template <int I, int X> friend struct MakeBoostRange;

static OmDimension ONE;
static OmDimension ZERO;

private:
	OmDimension<D-1> subDimension_;
	int              dimValue_    ;
};


template <int D>
OmDimension<D> OmDimension<D>::ONE  = OmDimension<D-1>::ONE[1];

template <int D>
OmDimension<D> OmDimension<D>::ZERO = OmDimension<D-1>::ZERO[0];


template <> struct OmDimension<0> {
public:
	OmDimension() {}
	boost::detail::multi_array::extent_gen<0> getBoostExtent() {
		return boost::extents;
	}

	OmDimension(OmDimension<1>) { assert(0); }

	OmDimension<1> operator[](int idx) const {
		return OmDimension<1>(*this, idx);
	}

  template <int I>
	inline int get() {
		assert(false);
		return 0;
	}

  template <int I>
	inline void set(int) {
		assert(false);
	}

  template <int I> OmDimension<0> stripDimension() {
	assert(false);
	return OmDimension<0>();
}

static const OmDimension<0> extent();

static const OmDimension<0> ONE();
static const OmDimension<0> ZERO();

};

//namespace {
//template<> OmDimension<0> OmDimension<0>::ONE  = OmDimension<0>();
//template<> OmDimension<0> OmDimension<0>::ZERO = OmDimension<0>();
//}


template <int I, int D> struct MakeBoostRange {
public:
	static boost::detail::multi_array::index_gen<I,D> make(OmDimension<I> from,
	                                                       OmDimension<I> to)
	{
		if (from.dimValue_ == to.dimValue_) {
			boost::detail::multi_array::index_gen<I-1,D> range =
			MakeBoostRange<I-1,D>::make
			(from.subDimension_, to.subDimension_);
			return range[from.dimValue_];
		}

		boost::detail::multi_array::index_gen<I-1,D-1> range =
		MakeBoostRange<I-1,D-1>::make
		(from.subDimension_, to.subDimension_);

		return range[boost::multi_array_types::index_range(from.dimValue_,
		             to.dimValue_)];
	}

	static boost::detail::multi_array::index_gen<I,D> makeLen(OmDimension<I> from,
	                                                          OmDimension<I> len)
	{
		boost::detail::multi_array::index_gen<I-1,D-1> range =
		MakeBoostRange<I-1,D-1>::makeLen
		(from.subDimension_, len.subDimension_);

		return range[boost::multi_array_types::index_range(from.dimValue_,
		             from.dimValue_ +
		             len.dimValue_)];
	}


};


template <int D> struct MakeBoostRange<0,D> {
public:
	static boost::detail::multi_array::index_gen<0,D> make(OmDimension<0>,
	                                                       OmDimension<0>)
	{
		return boost::detail::multi_array::index_gen<0,D>();
	}

	static boost::detail::multi_array::index_gen<0,D> makeLen(OmDimension<0>,
	                                                          OmDimension<0>)
	{
		return boost::detail::multi_array::index_gen<0,D>();
	}

};

namespace {
	static OmDimension<0> OmExtents;
}


