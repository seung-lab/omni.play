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

#ifndef ZI_MESHING_RTM_HPP
#define ZI_MESHING_RTM_HPP

#include "types.hpp"
#include "files.hpp"
#include "chunk.hpp"
#include "mesh.hpp"

#include <zi/bits/hash.hpp>
#include <zi/utility/for_each.hpp>
#include <zi/concurrency.hpp>

#include <map>
#include <set>

namespace zi {
namespace mesh {

class rtm
{
private:
    zi::rwmutex mutex_;

    std::map<vec5u, std::size_t> version_;
    zi::mutex                    version_mutex_;

    std::map<vec3u, std::set<uint32_t> > to_remesh_;
    zi::mutex                            to_remesh_mutex_;

    zi::task_manager::simple             remesh_tm_      ;

    std::map<vec3u, std::set<uint32_t> > to_remesh_next_ ;
    zi::mutex                            to_remesh_next_mutex_;


public:
    rtm()
        : mutex_()
        , version_()
        , version_mutex_()
        , to_remesh_()
        , to_remesh_mutex_()
        , remesh_tm_(20)
        , to_remesh_next_()
        , to_remesh_next_mutex_()
    { }

private:
    void schedule_remesh( const vec3u& c, uint32_t i )
    {
        zi::mutex::guard g(to_remesh_next_mutex_);
        to_remesh_next_[c].insert(i);
    }

    void schedule_remesh_around( const vec3u& c, uint32_t i )
    {
        uint32_t fx = ( c[0] > 0 ) ? c[0] - 1 : c[0];
        uint32_t fy = ( c[1] > 0 ) ? c[1] - 1 : c[1];
        uint32_t fz = ( c[2] > 0 ) ? c[2] - 1 : c[2];

        zi::mutex::guard g(to_remesh_next_mutex_);

        for ( uint32_t x = fx; x <= c[0] + 1; ++x )
        {
            for ( uint32_t y = fy; y <= c[1] + 1; ++y )
            {
                for ( uint32_t z = fz; z <= c[2] + 1; ++z )
                {
                    to_remesh_next_[vec3u(x,y,z)].insert(i);
                }
            }
        }
    }

    void get_scheduled( const vec3u&c, std::set<uint32_t>& s )
    {
        zi::mutex::guard g(to_remesh_mutex_);
        std::swap(to_remesh_[c], s);
        //to_remesh_.erase(c);
    }

    void swap_to_remesh()
    {
        zi::mutex::guard g1(to_remesh_mutex_);
        zi::mutex::guard g2(to_remesh_next_mutex_);
        std::swap( to_remesh_, to_remesh_next_ );
        to_remesh_next_.clear();
    }

public:
    std::size_t get_version( const vec5u& c )
    {
        zi::mutex::guard g(version_mutex_);
        return version_[c];
    }

    std::size_t inc_version( const vec5u& c )
    {
        zi::mutex::guard g(version_mutex_);
        return ++version_[c];
    }

private:
    std::size_t hashes( std::map<uint32_t, std::size_t>& ret,
                        const uint32_t* data, std::size_t len )
    {
        zi::hash<std::size_t> hasher;
        std::map<uint32_t, std::size_t> r;

        for ( std::size_t i = 0; i < len; ++i )
        {
            if ( data[i] )
            {
                std::size_t seed = r[data[i]];
                r[data[i]] ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            }
        }
        r.swap(ret);
        return ret.size();
    }

    void printit( )
    {
        std::cout << "PRINT: " << 123 << "\n";
    }

    void subvolume_update( vec3u coor,
                           vec3u from,
                           vec3u to,
                           chunk_ref_ptr d )
    {
        uint32_t x0 = coor[0];
        uint32_t y0 = coor[1];
        uint32_t z0 = coor[2];

        uint32_t fx = from[0];
        uint32_t fy = from[1];
        uint32_t fz = from[2];

        uint32_t tx = to[0];
        uint32_t ty = to[1];
        uint32_t tz = to[2];

        ZI_ASSERT(fx/128==tx/128);
        ZI_ASSERT(fy/128==ty/128);
        ZI_ASSERT(fz/128==tz/128);

        vec3u chunk_coord(fx / 128, fy / 128, fz / 128 );
        std::map<uint32_t,std::size_t> old_hashes, new_hashes;
        chunk_type_ptr chunk;

        // get old chunk and old hashes!
        chunk = chunk_io.read(chunk_coord, true);
        hashes(old_hashes, chunk->data(), chunk->num_elements());

        // update the chunk
        chunk->operator[](indices[range(fz%128,tz%128+1)][range(fy%128,ty%128+1)][range(fx%128,tx%128+1)])
            = d->operator[](indices[range(fz-z0,tz-z0+1)][range(fy-y0,ty-y0+1)][range(fx-x0,tx-x0+1)]);


        std::cout << "Filling Chunk: " << chunk_coord << " range: "
                  << "[ " << (fx%128) << "-" << (tx%128) << " "
                  << ", " << (fy%128) << "-" << (ty%128) << " "
                  << ", " << (fz%128) << "-" << (tz%128) << " ]" << std::endl;


        // get new hashes
        hashes(new_hashes, chunk->data(), chunk->num_elements());

        // save the new chunk
        chunk_io.write(chunk, chunk_coord);

        {
            FOR_EACH( it, new_hashes )
            {
                if ( it->first == 0 ) continue;

                if ( old_hashes.count( it->first ) == 0 )
                {
                    schedule_remesh_around(chunk_coord, it->first);
                }
                else
                {
                    if ( old_hashes[it->first] != it->second )
                    {
                        schedule_remesh_around(chunk_coord, it->first);
                    }
                }
            }

            FOR_EACH( it, old_hashes )
            {
                if ( it->first == 0 ) continue;

                if ( new_hashes.count( it->first ) == 0 )
                {
                    schedule_remesh_around(chunk_coord, it->first);
                }
            }
        }
    }

    void chunk_update_internal( vec3u chunk_coord, chunk_ref_ptr d )
    {
        std::map<uint32_t,std::size_t> old_hashes, new_hashes;
        chunk_type_ptr chunk;

        // get old chunk and old hashes!
        chunk = chunk_io.read(chunk_coord, true);
        hashes(old_hashes, chunk->data(), chunk->num_elements());

        // update the chunk
        chunk->operator[](indices[range(0,128)][range(0,128)][range(0,128)])
            = d->operator[](indices[range(0,128)][range(0,128)][range(0,128)]);


        // get new hashes
        hashes(new_hashes, chunk->data(), chunk->num_elements());

        // save the new chunk
        chunk_io.write(chunk, chunk_coord);

        {
            FOR_EACH( it, new_hashes )
            {
                if ( it->first == 0 ) continue;

                if ( old_hashes.count( it->first ) == 0 )
                {
                    schedule_remesh_around(chunk_coord, it->first);
                }
                else
                {
                    if ( old_hashes[it->first] != it->second )
                    {
                        schedule_remesh_around(chunk_coord, it->first);
                    }
                }
            }

            FOR_EACH( it, old_hashes )
            {
                if ( it->first == 0 ) continue;

                if ( new_hashes.count( it->first ) == 0 )
                {
                    schedule_remesh_around(chunk_coord, it->first);
                }
            }
        }
    }

    void volume_update_internal(uint32_t x, uint32_t y, uint32_t z,
                                uint32_t w, uint32_t h, uint32_t d,
                                chunk_ref_ptr c)
    {
        uint32_t x0 = x;
        uint32_t y0 = y;
        uint32_t z0 = z;
        uint32_t x1 = x + w - 1;
        uint32_t y1 = y + h - 1;
        uint32_t z1 = z + d - 1;

        uint32_t mx0 = x0 / 128;
        uint32_t my0 = y0 / 128;
        uint32_t mz0 = z0 / 128;
        uint32_t mx1 = x1 / 128;
        uint32_t my1 = y1 / 128;
        uint32_t mz1 = z1 / 128;

        remesh_tm_.start();

        uint32_t total_volume = 0;

        for ( uint32_t i = mx0; i <= mx1; ++i )
        {
            for ( uint32_t j = my0; j <= my1; ++j )
            {
                for ( uint32_t k = mz0; k <= mz1; ++k )
                {
                    uint32_t fromx = std::max( x0, i * 128 );
                    uint32_t tox   = std::min( x1, (i+1) * 128 - 1 );

                    uint32_t fromy = std::max( y0, j * 128 );
                    uint32_t toy   = std::min( y1, (j+1) * 128 - 1 );

                    uint32_t fromz = std::max( z0, k * 128 );
                    uint32_t toz   = std::min( z1, (k+1) * 128 - 1 );

                    remesh_tm_.insert( zi::run_fn(
                                           zi::bind( &rtm::subvolume_update, this,
                                                     vec3u(x,y,z), vec3u(fromx, fromy, fromz),
                                                     vec3u(tox, toy, toz), c ) ) );

                    total_volume += (tox-fromx+1)*(toz-fromz+1)*(toy-fromy+1);
                    // subvolume_update( vec3u(x,y,z), vec3u(fromx, fromy, fromz),
                    //                   vec3u(tox, toy, toz), c );


                    // std::cout << "Update MIP0; " << i << ' ' << j << ' ' << k
                    //           << "\n"
                    //           << "    [" << fromx << ' ' << tox << "]\n"
                    //           << "    [" << fromy << ' ' << toy << "]\n"
                    //           << "    [" << fromz << ' ' << toz << "]\n";
                }
            }
        }

        remesh_tm_.join();
        std::cout << "Total remeshed volume: " << total_volume
                  << " and should be " << (w*h*d) << std::endl;
    }

private:
    void process_chunk( const vec3u& c )
    {
        std::set<uint32_t> ids;
        get_scheduled(c, ids);

        chunk_type_ptr chunk = chunk_io.read_extended(c);

        zi::mesh::marching_cubes<int32_t> mc;
        mc.marche(reinterpret_cast<int32_t*>(chunk->data()), 129, 129, 129);

        //std::cout << "Processing Chunk: " << c << "\n";

        FOR_EACH( it, ids )
        {
            vec5u mcoord( c[0], c[1], c[2], 0, *it);

            //std::cout << "Processing Chunk Coord: " << mcoord << "\n";

            if ( mc.count(*it) )
            {
                int_mesh im;
                im.add(mc.get_triangles(*it));

                simplifier<double> s;
                im.fill_simplifier<double>(s);

                s.prepare();
                s.optimize(s.face_count()/10, 1e-12 );

                std::vector< zi::vl::vec3d > points ;
                std::vector< zi::vl::vec3d > normals;
                std::vector< vec3u >         faces  ;

                s.get_faces(points, normals, faces);
                mesh_type_ptr fmd( new mesh_type );
                fmd->add(points, normals, faces);

                std::cout << "Saving Chunk Coord: " << mcoord << "\n";

                mesh_io.write(fmd, mcoord);
                mesh_io.save_degenerate(mcoord, s);
            }
            else
            {
                mesh_io.remove(mcoord);
            }

            inc_version(mcoord);

            vec3u next(c[0]/2, c[1]/2, c[2]/2);
            schedule_remesh(next, *it);
        }

    }

    void process_chunk_mipn( const vec3u& c, uint32_t mip )
    {
        std::set<uint32_t> ids;
        get_scheduled(c, ids);

        FOR_EACH( it, ids )
        {
            vec5u mcoord( c[0], c[1], c[2], mip, *it);

            std::cout << "Processing Chunk Coord: " << mcoord << "\n";

            vl::vec3d offset = vl::vec3d(128<<mip,128<<mip,128<<mip);
            mesh_type_ptr fmd = mesh_io.read_from_lower(mcoord, offset);

            if ( fmd )
            {
                simplifier<double> s;
                fmd->fill_simplifier<double>(s);

                s.prepare();
                s.optimize(s.face_count()/2, (1<<(mip-1)));

                std::vector< zi::vl::vec3d > points ;
                std::vector< zi::vl::vec3d > normals;
                std::vector< vec3u >         faces  ;

                s.get_faces(points, normals, faces);
                mesh_type_ptr nfmd( new mesh_type );
                nfmd->add(points, normals, faces);

                std::cout << "Saving Chunk Coord: " << mcoord << "\n";

                mesh_io.write(nfmd, mcoord);
                mesh_io.save_degenerate(mcoord, s);

            }
            else
            {
                mesh_io.remove(mcoord);
            }

            inc_version(mcoord);

            vec3u next(c[0]/2, c[1]/2, c[2]/2);
            schedule_remesh(next, *it);
        }

    }

public:
    void volume_update(uint32_t x, uint32_t y, uint32_t z,
                       uint32_t w, uint32_t h, uint32_t d,
                       const char* data)
    {
        chunk_ref_ptr c( new chunk_type_ref(reinterpret_cast<const uint32_t*>(data),
                                            extents[d][h][w]) );

        to_remesh_.clear();
        volume_update_internal(x, y, z, w, h, d, c);

        swap_to_remesh();
        remesh_tm_.start();

        FOR_EACH( it, to_remesh_ )
        {
            remesh_tm_.insert( zi::run_fn( zi::bind( &rtm::process_chunk, this, it->first ) ) );
            //process_chunk( it->first );
            //remesh_tm_.insert( zi::run_fn( zi::bind( &rtm::printit, this ) ) );
        }

        remesh_tm_.join();

        for ( uint32_t mip = 1; mip < 9; ++mip )
        {
            swap_to_remesh();
            remesh_tm_.start();

            FOR_EACH( it, to_remesh_ )
            {
                remesh_tm_.insert( zi::run_fn( zi::bind( &rtm::process_chunk_mipn,
                                                         this, it->first, mip )));
                //process_chunk_mipn( it->first, mip );
            }

            remesh_tm_.join();
        }
    }

    void chunk_update( uint32_t x, uint32_t y, uint32_t z,
                       const char* data)
    {
        chunk_ref_ptr c( new chunk_type_ref(reinterpret_cast<const uint32_t*>(data),
                                            extents[128][128][128]) );

        to_remesh_.clear();
        chunk_update_internal( vec3u(x,y,z), c );

        swap_to_remesh();
        remesh_tm_.start();

        FOR_EACH( it, to_remesh_ )
        {
            remesh_tm_.insert( zi::run_fn( zi::bind( &rtm::process_chunk, this, it->first ) ) );
        }

        remesh_tm_.join();

        for ( uint32_t mip = 1; mip < 9; ++mip )
        {
            swap_to_remesh();
            remesh_tm_.start();

            FOR_EACH( it, to_remesh_ )
            {
                remesh_tm_.insert( zi::run_fn( zi::bind( &rtm::process_chunk_mipn,
                                                         this, it->first, mip )));
            }

            remesh_tm_.join();
        }
    }


}; // class rtm

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_RTM_HPP
