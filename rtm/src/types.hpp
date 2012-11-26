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

#ifndef ZI_MESHING_TYPES_HPP
#define ZI_MESHING_TYPES_HPP

#include <zi/vl/vl.hpp>
#include <zi/mesh/int_mesh.hpp>
#include <zi/mesh/face_mesh.hpp>
#include <zi/cstdint.hpp>

#include <boost/multi_array.hpp>
#include <boost/multi_array/types.hpp>
#include <boost/shared_ptr.hpp>

#define CHUNK_SIZE 128
#define CHUNK_DATA_SIZE (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)

namespace zi {
namespace mesh {

typedef zi::vl::vec<uint32_t, 3> vec3u;
typedef zi::vl::vec<uint32_t, 4> vec4u;
typedef zi::vl::vec<uint32_t, 5> vec5u;

using zi::vl::vec3d;
using zi::vl::vec3f;

typedef boost::shared_ptr<int_mesh>               int_mesh_ptr  ;
typedef boost::multi_array<uint32_t,3>            chunk_type    ;
typedef boost::const_multi_array_ref<uint32_t,3>  chunk_type_ref;
typedef boost::multi_array<char,3>                mask_type     ;
typedef boost::const_multi_array_ref<char,3>      mask_type_ref ;
typedef chunk_type::index_range                   range         ;
typedef boost::shared_ptr<chunk_type>             chunk_type_ptr;
typedef boost::shared_ptr<mask_type>              mask_type_ptr ;
typedef boost::shared_ptr<face_mesh<double> >     face_mesh_ptr ;
typedef boost::shared_ptr<chunk_type_ref>         chunk_ref_ptr ;
typedef boost::shared_ptr<mask_type_ref>          mask_ref_ptr  ;

typedef face_mesh<double>                         mesh_type     ;
typedef boost::shared_ptr<mesh_type>              mesh_type_ptr ;

namespace {

boost::multi_array_types::extent_gen extents;
boost::multi_array_types::index_gen  indices;

}


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_TYPES_HPP
