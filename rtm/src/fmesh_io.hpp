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

#ifndef ZI_MESHING_FMESH_IO_HPP
#define ZI_MESHING_FMESH_IO_HPP

#include "types.hpp"
#include "files.hpp"

#include <zi/utility/non_copyable.hpp>
#include <zi/concurrency.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
#include <cerrno>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include <zlib.h>

#include "fmesh_cache.hpp"

namespace zi {
namespace mesh {

class fmesh_io: non_copyable
{
private:
    uint32_t    id_    ;
    std::string prefix_;

    //std::map<vec4u, mesh_type_ptr> map_;
    //zi::mutex                      m_  ;

public:
    fmesh_io( uint32_t id )
        : id_(id)
        , prefix_()
          //, map_()
          //, m_()
    {
        file_io.create_dir("./data/" + boost::lexical_cast<std::string>(id));
        prefix_ = "./data/" + boost::lexical_cast<std::string>(id) + "/fmesh";
    }

    ~fmesh_io()
    {
        LOG(out) << "      fmesh_io for " << id_ << " died";
    }


private:

    bool add_to_mesh( mesh_type_ptr m, const vec4u& c,
                      double x = 0, double y = 0, double z = 0 )
    {
        std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/fmesh")
                                    % prefix_ % c[3] % c[0] % c[1] % c[2] );

        mesh_type_ptr toadd = fmesh_cache.get(id_, c);

        if ( toadd )
        {
            m->add(toadd->points(), toadd->normals(), toadd->faces(), x, y, z);
            return true;
        }

        //std::cout << "Trying to open: " << c << " :: " << s << "\n";
        std::ifstream in(s.c_str(), std::ios::in | std::ios::binary);
        if (in)
        {
            std::size_t tot = file_io.size(s);
            if ( tot == 0 )
            {
                return false;
            }

            char* buff = new char[tot];
            std::copy( std::istreambuf_iterator<char>(in),
                       std::istreambuf_iterator<char>(), buff );

            char* curr = buff;

            std::size_t* sizes = reinterpret_cast<std::size_t*>(curr);
            curr += sizeof( std::size_t ) * 2;

            mesh_type::point_t* pts = reinterpret_cast<mesh_type::point_t*>(curr);
            curr += sizeof( mesh_type::point_t) * sizes[0];

            mesh_type::point_t* nps =
                reinterpret_cast<mesh_type::point_t*>(curr);
            curr += sizeof( mesh_type::point_t ) * sizes[0];

            mesh_type::face_t* fcs =
                reinterpret_cast<mesh_type::face_t*>(curr);

            toadd = mesh_type_ptr( new mesh_type );
            toadd->add(pts, nps, sizes[0], fcs, sizes[1], x, y, z);
            m->add(pts, nps, sizes[0], fcs, sizes[1], x, y, z);

            fmesh_cache.set(id_, c, toadd);

            // {
            //     zi::mutex::guard g(m_);
            //     map_[c] = toadd;
            // }

            delete [] buff;

            //std::cout << "READ: " << tot << "\n";

            return true;
        }

        return 0;
    }

public:
    mesh_type_ptr read(const vec4u& c)
    {
        mesh_type_ptr m( new mesh_type );
        if ( !add_to_mesh(m, c) )
        {
            m.reset();
        }
        return m;
    }

    void remove(const vec4u& c)
    {
        std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/fmesh")
                                    % prefix_ % c[3] % c[0] % c[1] % c[2] );
        file_io.remove(s);

        fmesh_cache.remove(id_,c);

        //fmesh_cache.clear();

        // {
        //     zi::mutex::guard g(m_);
        //     map_.erase(c);
        // }
    }

    bool write(mesh_type_ptr m, const vec4u& c)
    {

        fmesh_cache.set(id_, c, m);
        // {
        //     zi::mutex::guard g(m_);
        //     map_[c] = m;
        // }

        vec4u cp(c[3], c[0], c[1], c[2]);
        if ( !file_io.create_path<4>(prefix_, cp) )
        {
            return 0;
            // maybe exception?
        }

        std::string s = boost::str( boost::format("%s/%d/%d/%d/%d/fmesh")
                                    % prefix_ % c[3] % c[0] % c[1] % c[2] );

        std::ofstream out(s.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
        if (out)
        {
            std::size_t sizes[2];
            sizes[0] = m->points().size();
            sizes[1] = m->faces().size();

            mesh_type::point_t* pbuff = new mesh_type::point_t[sizes[0]];
            mesh_type::point_t* nbuff = new mesh_type::point_t[sizes[0]];
            mesh_type::face_t*  fbuff = new mesh_type::face_t[sizes[1]];

            std::copy( m->points().begin(), m->points().end(), pbuff );
            std::copy( m->normals().begin(), m->normals().end(), nbuff );
            std::copy( m->faces().begin(), m->faces().end(), fbuff );

            out.write(reinterpret_cast<char*>(sizes),
                      static_cast<std::streamsize>(2*sizeof(std::size_t)));

            out.write(reinterpret_cast<char*>(pbuff),
                      static_cast<std::streamsize>(sizes[0]*sizeof(mesh_type::point_t)));

            out.write(reinterpret_cast<char*>(nbuff),
                      static_cast<std::streamsize>(sizes[0]*sizeof(mesh_type::point_t)));

            out.write(reinterpret_cast<char*>(fbuff),
                      static_cast<std::streamsize>(sizes[1]*sizeof(mesh_type::face_t)));

            delete [] pbuff;
            delete [] nbuff;
            delete [] fbuff;

            return true;
        }
        else
        {
            return false;
        }
    }

    mesh_type_ptr read_from_lower( const vec4u& c, const vl::vec3d& off )
    {
        if ( c[3] == 0 )
        {
            return read(c);
        }

        mesh_type_ptr m( new mesh_type );

        for ( uint32_t dx = 0; dx < 2; ++dx )
        {
            for ( uint32_t dy = 0; dy < 2; ++dy )
            {
                for ( uint32_t dz = 0; dz < 2; ++dz )
                {
                    mesh_type_ptr x = read(vec4u(c[0]*2+dx,c[1]*2+dy,c[2]*2+dz,c[3]-1));
                    if ( x )
                    {
                        m->add(x, off[0]*dz, off[1]*dy, off[2]*dx);
                    }
                }
            }
        }

        if ( m->size() == 0 )
        {
            m.reset();
        }

        return m;
    }

    void erase_all()
    {
        file_io.remove_dir(prefix_);
        fmesh_cache.clear();
    }


}; // class fmesh_io


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_FMESH_IO_HPP
