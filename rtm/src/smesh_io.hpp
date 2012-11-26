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

#ifndef ZI_MESHING_SMESH_IO_HPP
#define ZI_MESHING_SMESH_IO_HPP

#include "types.hpp"
#include "files.hpp"
#include "detail/rwlock_pool.hpp"
#include "detail/vec_hash.hpp"

#include <zi/utility/non_copyable.hpp>
#include <zi/bits/hash.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
#include <cerrno>
#include <string>
#include <vector>
#include <cstdlib>

#include <zlib.h>

namespace zi {
namespace mesh {

class smesh_io: non_copyable
{
private:
    typedef rwlock_pool_n< vec4u, 93, vec_hash<vec4u> >   rwlock_pool_type;
    typedef lock_pool_read_guard<rwlock_pool_type>        read_guard      ;
    typedef lock_pool_write_guard<rwlock_pool_type>       write_guard     ;

private:
    uint32_t                               id_          ;
    std::string                            prefix_      ;
    rwlock_pool_type                       lock_pool_   ;
    mutable std::map<vec4u, std::size_t>   hashes_      ;
    zi::mutex                              hashes_mutex_;

public:
    smesh_io( uint32_t id )
        : id_(id)
        , prefix_()
        , lock_pool_()
        , hashes_()
        , hashes_mutex_()
    {
        file_io.create_dir("./data/" + boost::lexical_cast<std::string>(id));
        prefix_ = "./data/" + boost::lexical_cast<std::string>(id) + "/smesh";
    }

    ~smesh_io()
    {
        LOG(out) << "      smesh_io for " << id_ << " died";
    }

private:
    std::size_t hash( const uint32_t* d, std::size_t l )
    {
        zi::hash<uint32_t> hasher;

        std::size_t seed = 0;
        for ( std::size_t i = 0; i < l; ++i )
        {
            seed ^= hasher(d[i]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }

private:
    void tri_strip_to_degenerate( std::vector<float>& newpoints,
                                  const std::vector< zi::vl::vec3d >& points,
                                  const std::vector< zi::vl::vec3d >& normals,
                                  const std::vector< uint32_t >& indices,
                                  const std::vector< uint32_t >& starts,
                                  const std::vector< uint32_t >& lengths )
    {

        newpoints.clear();

        for ( std::size_t i = 0; i < starts.size(); ++i )
        {
            if ( i > 0 )
            {
                // add the last point
                {
                    std::size_t idx = indices[starts[i-1] + lengths[i-1] - 1];
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                }

                if ( (newpoints.size()/6) % 2 == 0 )
                {
                    std::size_t idx = indices[starts[i]];
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                }

                {
                    std::size_t idx = indices[starts[i]];
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                }
            }

            for ( uint32_t j = starts[i]; j < starts[i] + lengths[i]; ++j )
            {
                std::size_t idx = indices[j];
                newpoints.push_back(static_cast<float>(points[idx].at(2)));
                newpoints.push_back(static_cast<float>(points[idx].at(1)));
                newpoints.push_back(static_cast<float>(points[idx].at(0)));
                newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                newpoints.push_back(static_cast<float>(normals[idx].at(0)));
            }

        }
    }

    void read_nl( const vec4u& c, std::string& ret )
    {
        std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/smesh")
                                    % prefix_ % c[3] % c[0] % c[1] % c[2] );

        std::FILE *fp = std::fopen(s.c_str(), "rb");

        if ( fp )
        {
            std::fseek(fp, 0, SEEK_END);
            ret.resize(std::ftell(fp));
            std::rewind(fp);
            if ( ret.size() != std::fread(reinterpret_cast<void*>(
                                              const_cast<char*>(ret.data())), 1, ret.size(), fp) )
            {
                LOG(error) << "Can't read whole smesh file: " << s;
            }
            std::fclose(fp);
        }
    }

    bool write_nl( const vec4u& c, simplifier<double>& simp )
    {
        std::vector< vl::vec3d > points;
        std::vector< vl::vec3d > normals;
        std::vector< uint32_t >  indices;
        std::vector< uint32_t >  strip_begins;
        std::vector< uint32_t >  strip_lengths;

        simp.stripify( points, normals, indices, strip_begins, strip_lengths );

        std::vector<float> degen;

        tri_strip_to_degenerate( degen, points, normals, indices, strip_begins, strip_lengths );

        vec4u cp(c[3], c[0], c[1], c[2]);
        if ( !file_io.create_path<4>(prefix_, cp) )
        {
            return 0;
            // maybe exception?
        }

        std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/smesh")
                                    % prefix_ % c[3] % c[0] % c[1] % c[2] );

        std::ofstream out(s.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
        if (out)
        {
            float* buff = new float[degen.size()];

            std::copy( degen.begin(), degen.end(), buff );

            out.write(reinterpret_cast<char*>(buff),
                      static_cast<std::streamsize>(degen.size()*sizeof(float)));

            std::size_t oh = get_hash_nl(c);
            std::size_t nh = hash(reinterpret_cast<uint32_t*>(buff), degen.size());

            set_hash_nl(c,nh);

            delete [] buff;

            return oh == nh;
        }
        else
        {
            return 0;
        }
    }

private:
    std::size_t get_hash_nl( const vec4u& c )
    {
        zi::mutex::guard g(hashes_mutex_);

        if ( hashes_.count(c) == 0 )
        {
            std::string s;
            read_nl(c,s);

            if ( s.size() == 0 )
            {
                hashes_[c] = 0;
            }
            else
            {
                hashes_[c] = hash(reinterpret_cast<const uint32_t*>(s.data()),
                                  s.size()/4);
            }
        }

        return hashes_[c];
    }

    void set_hash_nl( const vec4u& c, std::size_t h )
    {
        zi::mutex::guard g(hashes_mutex_);
        hashes_[c] = h;
    }

public:
    std::size_t read( const vec4u& c, std::string& ret )
    {
        read_guard g(lock_pool_, c);
        read_nl(c,ret);
        return get_hash_nl(c);
    }

    std::size_t read_if_different( const vec4u& c, std::string& ret, std::size_t h )
    {
        read_guard g(lock_pool_, c);
        std::size_t r = get_hash_nl(c);

        if ( r != h )
        {
            read_nl(c,ret);
        }

        return r;
    }

    bool remove(const vec4u& c)
    {
        write_guard g(lock_pool_, c);

        std::size_t h = get_hash_nl(c);

        if ( h )
        {
            std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/smesh")
                                        % prefix_ % c[3] % c[0] % c[1] % c[2] );
            file_io.remove(s);
            set_hash_nl(c,0);
            return 1;
        }

        return 0;
    }

    std::size_t write( const vec4u& c, simplifier<double>& simp )
    {
        write_guard g(lock_pool_, c);
        return write_nl(c,simp);
    }

    std::size_t get_hash( const vec4u& c )
    {
        read_guard g(lock_pool_, c);
        return get_hash_nl(c);
    }

    void erase_all()
    {
        zi::mutex::guard g(hashes_mutex_);
        file_io.remove_dir(prefix_);
        {
            hashes_.clear();
        }
    }

}; // class smesh_io



} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_SMESH_IO_HPP
