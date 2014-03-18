#pragma once
#include "precomp.h"

namespace om {

template <typename T>
class singletonBase : public zi::enable_singleton_of_this<T> {};

};  // om
