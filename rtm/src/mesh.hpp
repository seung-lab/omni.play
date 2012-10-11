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

#ifndef ZI_MESHING_MESH_HPP
#define ZI_MESHING_MESH_HPP

#include "types.hpp"
#include "files.hpp"

#include <zi/utility/singleton.hpp>

#include <boost/format.hpp>

#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
#include <cerrno>
#include <string>
#include <vector>

#include <zlib.h>

namespace zi {
namespace mesh {

class mesh_io_impl
{
private:
    bool add_to_mesh( mesh_type_ptr m, const vec5u& c,
                      double x = 0, double y = 0, double z = 0 ) const
    {
        std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.fmesh")
                                    % c[3] % c[0] % c[1] % c[2] % c[4] );

        std::cout << "Trying to open: " << c << " :: " << s << "\n";
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

            m->add(pts, nps, sizes[0], fcs, sizes[1], x, y, z);

            delete [] buff;

            std::cout << "READ: " << tot << "\n";

            return tot;
        }

        return 0;
    }

public:
    mesh_type_ptr read(const vec5u& c) const
    {
        mesh_type_ptr m( new mesh_type );
        if ( !add_to_mesh(m, c) )
        {
            m.reset();
        }

        return m;
    }

    void remove(const vec5u& c) const
    {
        std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.fmesh")
                                    % c[3] % c[0] % c[1] % c[2] % c[4] );
        file_io.remove(s);

        s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.smesh")
                        % c[3] % c[0] % c[1] % c[2] % c[4] );
        file_io.remove(s);
    }

    bool write(mesh_type_ptr m, const vec5u& c) const
    {
        vec4u cp(c[3], c[0], c[1], c[2]);
        if ( !file_io.create_path<4>("./data/fmesh", cp) )
        {
            return false;
        }

        std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.fmesh")
                                    % c[3] % c[0] % c[1] % c[2] % c[4] );

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

    mesh_type_ptr read_from_lower( const vec5u& c, const vl::vec3d& off ) const
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
                    mesh_type_ptr x = read(vec5u(c[0]*2+dx,c[1]*2+dy,c[2]*2+dz,c[3]-1,c[4]));
                    if ( x )
                    {
                        m->add(x, off[0]*dx, off[1]*dy, off[2]*dz);
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

public:
    void tri_strip_to_degenerate( std::vector<float>& newpoints,
                                  const std::vector< zi::vl::vec3d >& points,
                                  const std::vector< zi::vl::vec3d >& normals,
                                  const std::vector< uint32_t >& indices,
                                  const std::vector< uint32_t >& starts,
                                  const std::vector< uint32_t >& lengths ) const
    {

        newpoints.clear();

        for ( std::size_t i = 0; i < starts.size(); ++i )
        {
            if ( i > 0 )
            {
                // add the last point
                {
                    std::size_t idx = indices[starts[i-1] + lengths[i-1] - 1];
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                }

                if ( (newpoints.size()/6) % 2 == 0 )
                {
                    std::size_t idx = indices[starts[i]];
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                }

                {
                    std::size_t idx = indices[starts[i]];
                    newpoints.push_back(static_cast<float>(points[idx].at(0)));
                    newpoints.push_back(static_cast<float>(points[idx].at(1)));
                    newpoints.push_back(static_cast<float>(points[idx].at(2)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                    newpoints.push_back(static_cast<float>(normals[idx].at(2)));
                }
            }

            for ( uint32_t j = starts[i]; j < starts[i] + lengths[i]; ++j )
            {
                std::size_t idx = indices[j];
                newpoints.push_back(static_cast<float>(points[idx].at(0)));
                newpoints.push_back(static_cast<float>(points[idx].at(1)));
                newpoints.push_back(static_cast<float>(points[idx].at(2)));
                newpoints.push_back(static_cast<float>(normals[idx].at(0)));
                newpoints.push_back(static_cast<float>(normals[idx].at(1)));
                newpoints.push_back(static_cast<float>(normals[idx].at(2)));
            }

        }
    }

    bool save_degenerate( const vec5u& c, simplifier<double>& simp ) const
    {
        std::vector< vl::vec3d > points;
        std::vector< vl::vec3d > normals;
        std::vector< uint32_t > indices;
        std::vector< uint32_t > strip_begins;
        std::vector< uint32_t > strip_lengths;

        simp.stripify( points, normals, indices, strip_begins, strip_lengths );

        std::vector<float> degen;

        tri_strip_to_degenerate( degen, points, normals, indices, strip_begins, strip_lengths );

        std::cout << "DEGEN SIZE: " << degen.size() << "\n";

        vec4u cp(c[3], c[0], c[1], c[2]);
        if ( !file_io.create_path<4>("./data/fmesh", cp) )
        {
            return false;
        }

        std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.smesh")
                                    % c[3] % c[0] % c[1] % c[2] % c[4] );

        std::ofstream out(s.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
        if (out)
        {
            float* buff = new float[degen.size()];

            std::copy( degen.begin(), degen.end(), buff );

            out.write(reinterpret_cast<char*>(buff),
                      static_cast<std::streamsize>(degen.size()*sizeof(float)));

            delete [] buff;

            return true;
        }
        else
        {
            return false;
        }
    }


    bool read_degenerate( const vec5u& c, std::string& ret ) const
    {
        std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.smesh")
                                    % c[3] % c[0] % c[1] % c[2] % c[4] );

        std::cout << "Trying to open: " << c << " :: " << s << "\n";
        std::ifstream in(s.c_str(), std::ios::in | std::ios::binary);
        if (in)
        {

            ret.assign( (std::istreambuf_iterator<char>(in) ),
                        (std::istreambuf_iterator<char>()) );

            std::cout << "READ: " << ret.size() << "\n";

            return true;
        }

        return 0;
    }



}; // class mesh_io_impl


namespace {

const mesh_io_impl& mesh_io = zi::singleton<mesh_io_impl>::instance();

} // namespace


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_MESH_HPP
