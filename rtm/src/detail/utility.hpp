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

#ifndef ZI_MESHING_DETAIL_UTILITY_HPP
#define ZI_MESHING_DETAIL_UTILITY_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <zi/cstdint.hpp>

#include <zi/vl/vl.hpp>

#include <boost/lexical_cast.hpp>


namespace zi {
namespace mesh {


template< uint32_t N >
inline bool create_path(const std::string& prefix, const zi::vl::vec<uint32_t, N>& v)
{
    struct stat s;

    std::string fname = ".";

    if ( prefix != "" )
    {
        fname = prefix;
        if ( ::stat(prefix.c_str(), &s ) )
        {
            if ( ::mkdir(prefix.c_str(), 0777) )
            {
                return false;
            }
        }
        else
        {
            if ( !S_ISDIR(s.st_mode) )
            {
                return false;
            }
        }
    }

    for ( uint32_t i = 0; i < N; ++i )
    {
        fname += '/';
        fname += boost::lexical_cast<std::string>(v[i]);
        if ( ::stat(fname.c_str(), &s ) )
        {
            if ( ::mkdir(fname.c_str(), 0777) )
            {
                return false;
            }
        }
        else
        {
            if ( !S_ISDIR(s.st_mode) )
            {
                return false;
            }
        }
    }

    return true;
}




} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_UTILITY_HPP
