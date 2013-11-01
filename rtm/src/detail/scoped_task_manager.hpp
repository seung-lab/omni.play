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

#ifndef ZI_MESHING_DETAIL_SCOPED_TASK_MANAGER_HPP
#define ZI_MESHING_DETAIL_SCOPED_TASK_MANAGER_HPP

#include <zi/concurrency.hpp>

#include <cstddef>

namespace zi {
namespace mesh {

class scoped_task_manager: public zi::task_manager::simple
{
public:
    scoped_task_manager( std::size_t n )
        : zi::task_manager::simple(n)
    {
        zi::task_manager::simple::start();
    }
    ~scoped_task_manager()
    {
        zi::task_manager::simple::join();
    }
}; // scoped_task_manager


} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_SCOPED_TASK_MANAGER_HPP
