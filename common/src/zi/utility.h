#pragma once

#include <zi/system.hpp>

#include <zi/singleton.hpp>
#include <zi/utility/enable_singleton_of_this.hpp>
#include <zi/for_each.hpp>
#include <zi/assert.hpp>

#include <zi/parallel/algorithm.hpp>
#include <zi/parallel/numeric.hpp>

namespace om {

template <typename T>
class SingletonBase : public zi::enable_singleton_of_this<T> {};

};  // om
