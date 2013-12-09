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

#ifndef ZI_MESHING_FILES_HPP
#define ZI_MESHING_FILES_HPP

#include "types.hpp"

#include <zi/utility/singleton.hpp>
#include <zi/concurrency.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <string>
#include <cstdlib>
#include <cstdio>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


namespace zi {
namespace mesh {

namespace file {

inline void ls( std::list<std::string>& ret, const std::string& dir_path )
{
    DIR* dp = opendir(dir_path.c_str());
    if ( dp == 0 )
    {
        return;
    }

    struct dirent *dirp;

    while ( (dirp = readdir(dp) ) != NULL )
    {
        if ( (strcmp(dirp->d_name, ".") != 0) &&
             (strcmp(dirp->d_name, "..") != 0) )
        {
            ret.push_back(std::string(dirp->d_name));
        }
    }

    if (closedir(dp) < 0)
    {
        return;
    }
}

inline void find_files( std::list<std::string>& ret, const std::string& dir_path )
{
    struct stat statbuf;

    if (lstat(dir_path.c_str(), &statbuf) < 0)
    {
        return;
    }

    if (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
        ret.push_back( dir_path );
        return;
    }

    if (!S_ISDIR(statbuf.st_mode))
    {
        return;
    }

    DIR* dp = opendir(dir_path.c_str());

    if (dp == 0)
    {
        return;
    }

    struct dirent *dirp;

    while ( (dirp = readdir(dp) ) != NULL )
    {
        if ( (strcmp(dirp->d_name, ".") != 0) &&
             (strcmp(dirp->d_name, "..") != 0) )
        {
            find_files(ret, dir_path + "/" + std::string(dirp->d_name));
        }
    }

    if (closedir(dp) < 0)
    {
        return;
    }

}

} // namespace file

namespace detail {

int remove_dir(const char* dir_path)
{

    struct stat statbuf;

    if (lstat(dir_path, &statbuf) < 0)
    {
        // doesn't exist - we are all set!
        return 0;
    }

    if (S_ISREG(statbuf.st_mode) || S_ISLNK(statbuf.st_mode))
    {
        // file or link - delete it
        return remove(dir_path);
    }

    if (!S_ISDIR(statbuf.st_mode))
    {
        // anything else -- error!
        return -1;
    }

    DIR* dp = opendir(dir_path);

    if (dp == 0)
    {
        return -1;
    }

    struct dirent *dirp;
    char buf[1024];

    while ( (dirp = readdir(dp) ) != NULL )
    {
        if ( (strcmp(dirp->d_name, ".") != 0) &&
             (strcmp(dirp->d_name, "..") != 0) )
        {
            sprintf(buf, "%s/%s", dir_path, dirp->d_name);
            if (remove_dir(buf))
            {
                return -1;
            }
        }
    }

    if (closedir(dp) < 0)
    {
        return -1;
    }

    if (remove(dir_path) < 0)
    {
        return -1;
    }

    return 0;
};

} // namespace detail

class file_io_impl
{
private:
    mutable zi::task_manager::simple tm_;
    mutable zi::mutex                m_ ;
    mutable boost::mt19937           rng_;
    mutable boost::variate_generator<boost::mt19937&, boost::uniform_real<> > rnd_;

public:
    file_io_impl()
        : tm_(10)
        , m_()
        , rng_(std::time(0))
        , rnd_( rng_, boost::uniform_real<>(0,1) )
    {
        tm_.start();
    }

    ~file_io_impl()
    {
        tm_.join();
    }

private:
    void remove_dir_in_background( const std::string& dir ) const
    {
        detail::remove_dir(dir.c_str());
    }

public:
    void remove_dir(const std::string& dir) const
    {
        zi::mutex::guard g(m_);
        std::string new_dir = "./" + boost::lexical_cast<std::string>
            (static_cast<double>(rnd_()));

        rename(dir.c_str(), new_dir.c_str());
        tm_.insert(zi::run_fn(zi::bind( &file_io_impl::remove_dir_in_background,
                                        this, new_dir) ));
    }

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

    bool create_dir(const std::string& fname) const
    {
        struct stat s;

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

        return true;
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
