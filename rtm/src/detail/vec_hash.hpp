#pragma once
#ifndef ZI_MESHING_DETAIL_VEC_HASH_HPP
#define ZI_MESHING_DETAIL_VEC_HASH_HPP

#include <zi/vl/vec.hpp>
#include <zi/bits/hash.hpp>

#include <functional>
#include <cstddef>

namespace zi {
namespace mesh {

template< typename T >
struct vec_hash;

template< typename T, std::size_t N >
struct vec_hash< zi::vl::vec<T,N> >
{
    std::size_t operator()(const zi::vl::vec<T,N>& v) const
    {
        zi::hash<std::size_t> hasher;

        std::size_t seed = 0;
        for ( std::size_t i = 0; i < N; ++i )
        {
            seed ^= hasher(v[i]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_VEC_HASH_HPP
