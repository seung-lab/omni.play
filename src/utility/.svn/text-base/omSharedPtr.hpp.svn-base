#pragma once

//
// Copyright (C) 2010  Aleksandar Zlateski <zlateski@mit.edu>
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

#include <zi/config/config.hpp>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#  include <memory>
#  define ZI_SHARED_PTR_NAMESPACE ::std
#else
#    include <boost/shared_ptr.hpp>
#    include <boost/make_shared.hpp>
#    define ZI_SHARED_PTR_NAMESPACE ::boost
#endif

namespace om {

using ZI_SHARED_PTR_NAMESPACE::shared_ptr ;
using ZI_SHARED_PTR_NAMESPACE::make_shared;
using ZI_SHARED_PTR_NAMESPACE::weak_ptr;

} //namespace om

#undef ZI_SHARED_PTR_NAMESPACE
