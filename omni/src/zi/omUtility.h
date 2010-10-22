#ifndef OM_UTILITY_HPP
#define OM_UTILITY_HPP

#include <zi/singleton.hpp>
#include <zi/utility/enable_singleton_of_this.hpp>
#include <zi/for_each.hpp>
#include <zi/assert.hpp>

namespace om {
	template <typename T>
	class singletonBase : public zi::enable_singleton_of_this<T> {};
};

#endif
