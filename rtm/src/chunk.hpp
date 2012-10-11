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

#ifndef ZI_MESHING_CHUNK_HPP
#define ZI_MESHING_CHUNK_HPP

#include "types.hpp"
#include "files.hpp"

#include <zi/utility/singleton.hpp>

#include <boost/format.hpp>

#include <string>

#include <zlib.h>

namespace zi {
namespace mesh {

class chunk_io_impl
{
public:
    chunk_type_ptr read(const vec3u& c, bool fill_zeros = false) const
    {
        std::string fname = boost::str(
            boost::format("./data/raw/%d/%d/%d/chunk.raw")
            % c[0] % c[1] % c[2] );

        gzFile fin;

        if ( (fin = gzopen(fname.c_str(), "r")) )
        {
            chunk_type_ptr ret( new chunk_type(extents[128][128][128]) );

            std::size_t r = gzread( fin, reinterpret_cast<void*>(ret->data()),
                                    sizeof(uint32_t)*128*128*128);
            gzclose(fin);

            std::cout << "GZREAD: " << r  << " / "
                      << (sizeof(uint32_t)*128*128*128) << "\n";

            if ( r == sizeof(uint32_t)*128*128*128 )
            {
                return ret;
            }
        }

        if ( fill_zeros )
        {
            return chunk_type_ptr( new chunk_type(extents[128][128][128]) );
        }
        else
        {
            return chunk_type_ptr();
        }
    }

    bool write(chunk_type_ptr v, const vec3u& c) const
    {
        if ( !file_io.create_path<3>("./data/raw", c) )
        {
            return false;
        }

        std::string fname = boost::str(
            boost::format("./data/raw/%d/%d/%d/chunk.raw") % c[0] % c[1] % c[2] );

        gzFile out;

        if ( (out = gzopen(fname.c_str(), "w")) )
        {
            std::size_t r = gzwrite(out, reinterpret_cast<const void*>(v->data()),
                                    sizeof(uint32_t)*128*128*128);

            std::cout << "GZWRITE: " << r  << " / "
                      << (sizeof(uint32_t)*128*128*128) << "\n";
            gzclose(out);
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    enum side_to_attach
    {
        no_side = 0,
        x_side,
        y_side,
        z_side,
        xy_side,
        yz_side,
        xz_side,
        xyz_side
    };

    void merge(chunk_type& v, const chunk_type& x, side_to_attach s) const
    {
        static const std::size_t N = 128;

        switch (s)
        {
        case z_side:
            v[indices[N][range(0,N)][range(0,N)]] = x[indices[0][range(0,N)][range(0,N)]];
            break;
        case y_side:
            v[indices[range(0,N)][N][range(0,N)]] = x[indices[range(0,N)][0][range(0,N)]];
            break;
        case x_side:
            v[indices[range(0,N)][range(0,N)][N]] = x[indices[range(0,N)][range(0,N)][0]];
            break;
        case yz_side:
            v[indices[N][N][range(0,N)]] = x[indices[0][0][range(0,N)]];
            break;
        case xy_side:
            v[indices[range(0,N)][N][N]] = x[indices[range(0,N)][0][0]];
            break;
        case xz_side:
            v[indices[N][range(0,N)][N]] = x[indices[0][range(0,N)][0]];
            break;
        case xyz_side:
            v[N][N][N] = x[0][0][0];
            break;
        case no_side:
            v[indices[range(0,N)][range(0,N)][range(0,N)]]
                = x[indices[range(0,N)][range(0,N)][range(0,N)]];
            break;
        default:
            return;
        }
    }

    void merge(chunk_type_ptr v, chunk_type_ptr x, side_to_attach s) const
    {
        merge(*v.get(), *x.get(), s);
    }

public:
    chunk_type_ptr read_extended(const vec3u& c, chunk_type_ptr v = chunk_type_ptr() ) const
    {
        static const std::size_t N = 129;

        chunk_type_ptr r(new chunk_type(extents[N][N][N]));

        if ( !v )
        {
            v = read(c);
        }

        if ( v )
        {
            merge(r, v, no_side);
        }

        vec3u nc = c; ++nc[0];
        v = read(nc);
        if ( v ) merge(r, v, x_side);

        nc = c; ++nc[1];
        v = read(nc);
        if ( v ) merge(r, v, y_side);

        nc = c; ++nc[2];
        v = read(nc);
        if ( v ) merge(r, v, z_side);

        nc = c; ++nc[1]; ++nc[2];
        v = read(nc);
        if ( v ) merge(r, v, yz_side);

        nc = c; ++nc[0]; ++nc[1];
        v = read(nc);
        if ( v ) merge(r, v, xy_side);

        nc = c; ++nc[0]; ++nc[2];
        v = read(nc);
        if ( v ) merge(r, v, xz_side);

        nc = c; ++nc[0]; ++nc[1]; ++nc[2];
        v = read(nc);
        if ( v ) merge(r, v, xyz_side);

        return r;

    }

}; // class chunk_io_impl


namespace {

const chunk_io_impl& chunk_io = zi::singleton<chunk_io_impl>::instance();

} // namespace


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_CHUNK_HPP
