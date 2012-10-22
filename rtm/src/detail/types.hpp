//
// Copyright (C) 2012  Aleksandar Zlateski <zlateski@mit.edu>
// ----------------------------------------------------------
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ZI_MESHING_DETAIL_TYPES_HPP
#define ZI_MESHING_DETAIL_TYPES_HPP

#include <zi/vl/vl.hpp>
#include <zi/mesh/int_mesh.hpp>
#include <zi/mesh/face_mesh.hpp>
#include <zi/bits/type_traits.hpp>

namespace zi {
namespace mesh {

typedef zi::vl::vec<uint16_t, 3> vec3us;
typedef zi::vl::vec<uint16_t, 4> vec4us;
typedef zi::vl::vec<uint16_t, 5> vec5us;

typedef zi::vl::vec<uint32_t, 3> vec3u;
typedef zi::vl::vec<uint32_t, 4> vec4u;
typedef zi::vl::vec<uint32_t, 5> vec5u;

typedef zi::vl::vec<uint64_t, 3> vec3lu;
typedef zi::vl::vec<uint64_t, 4> vec4lu;
typedef zi::vl::vec<uint64_t, 5> vec5lu;

typedef zi::vl::vec<int16_t, 3> vec3s;
typedef zi::vl::vec<int16_t, 4> vec4s;
typedef zi::vl::vec<int16_t, 5> vec5s;

typedef zi::vl::vec<int32_t, 3> vec3i;
typedef zi::vl::vec<int32_t, 4> vec4i;
typedef zi::vl::vec<int32_t, 5> vec5i;

typedef zi::vl::vec<int64_t, 3> vec3li;
typedef zi::vl::vec<int64_t, 4> vec4li;
typedef zi::vl::vec<int64_t, 5> vec5li;

typedef zi::vl::vec<float, 3> vec3f;
typedef zi::vl::vec<float, 4> vec4f;
typedef zi::vl::vec<float, 5> vec5f;

typedef zi::vl::vec<double, 3> vec3d;
typedef zi::vl::vec<double, 4> vec4d;
typedef zi::vl::vec<double, 5> vec5d;

typedef zi::vl::vec<long double, 3> vec3ld;
typedef zi::vl::vec<long double, 4> vec4ld;
typedef zi::vl::vec<long double, 5> vec5ld;

typedef boost::shared_ptr<int_mesh>           int_mesh_ptr  ;
typedef boost::multi_array<uint32_t,3>        chunk_type    ;
typedef boost::shared_ptr<chunk_type>         chunk_type_ptr;
typedef boost::shared_ptr<face_mesh<double> > face_mesh_ptr ;

template< class T > struct vec_traits;

template< class T > struct vec_traits
: vec_traits<typename remove_const<T>::type> {};

template< class T > struct vec_traits
: vec_traits<typename remove_reference<T>::type> {};

template< class T, std::size_t N >
struct vec_traits< zi::vl::vec<T,N> >
{
    typedef T                value_type;
    static const std::size_t length = N;
};



} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_DETAIL_TYPES_HPP
