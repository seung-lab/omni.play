#pragma once

#include "precomp.h"

namespace om {

template <typename T>
class SingletonBase : public zi::enable_singleton_of_this<T> {};

};  // om
