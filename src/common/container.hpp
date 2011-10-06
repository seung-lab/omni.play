#pragma once

#include "common/std.h"

#include <zi/for_each.hpp>
#include <zi/utility/container_utilities.hpp>

namespace om {
namespace container {

using zi::containers::clear;

template <typename T>
void eraseRemove(std::vector<T>& v, const T val){
    v.erase(std::remove(v.begin(), v.end(), val), v.end());
}

template <typename U>
void clearPtrVec(std::vector<U*>& vec)
{
    FOR_EACH(iter, vec){
        delete *iter;
    }
}

} // namespace container
} // namespace om

