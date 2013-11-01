#pragma once
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

#ifndef ZI_MESHING_DETAIL_ASSERT_HPP
#define ZI_MESHING_DETAIL_ASSERT_HPP

#include "log.hpp"

#include <zi/zpp/stringify.hpp>
#include <csignal>

#define ASSERT_ERROR(cond)                                      \
    if ( !(cond) )                                              \
    {                                                           \
        LOG(error) << "File: " << __FILE__                      \
                   << ":" << __LINE__ << ' '                    \
                   << "In: " << __PRETTY_FUNCTION__ << " ("     \
                   << ZiPP_STRINGIFY(cond) << ')';              \
    }                                                           \
    (static_cast<void>(0))

#define ASSERT_FATAL(cond)                                      \
    if ( !(cond) )                                              \
    {                                                           \
        LOG(fatal) << "File: " << __FILE__                      \
                   << ":" << __LINE__ << ' '                    \
                   << "In: " << __PRETTY_FUNCTION__ << " ("     \
                   << ZiPP_STRINGIFY(cond) << ')';              \
        raise(9);                                               \
    }                                                           \
    (static_cast<void>(0))

#endif // ZI_MESHING_DETAIL_ASSERT_HPP
