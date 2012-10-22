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

#ifndef ZI_MESHING_FILES_HPP
#define ZI_MESHING_FILES_HPP

#include "types.hpp"

#include <zi/utility/singleton.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace zi {
namespace mesh {

class file_io_impl
{
public:
    bool exists(const std::string& name) const
    {
        struct stat s;
        return ::stat(name.c_str(), &s) == 0;
    }

    bool remove(const std::string& name) const
    {
        return ::unlink(name.c_str()) == 0;
    }

    std::size_t size(const std::string& fname) const
    {
        struct stat filestatus;
        if ( ::stat(fname.c_str(), &filestatus) == 0 )
        {
            return static_cast<std::size_t>(filestatus.st_size);
        }
        return 0;
    }

    template< uint32_t N >
    bool create_path(const std::string& prefix, const zi::vl::vec<uint32_t, N>& v) const
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

};

namespace {

const file_io_impl& file_io = zi::singleton<file_io_impl>::instance();

} // namespace

} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_FILES_HPP
