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

#ifndef ZI_MESHING_INTERACTIVE_MESH_HPP
#define ZI_MESHING_INTERACTIVE_MESH_HPP

#include "detail/log.hpp"
#include "detail/assert.hpp"
#include "detail/scoped_task_manager.hpp"

#include "types.hpp"
#include "files.hpp"
#include "chunk_mem_io.hpp"
#include "fmesh_io.hpp"
#include "smesh_io.hpp"

#include <zi/bits/hash.hpp>
#include <zi/bits/array.hpp>
#include <zi/utility/for_each.hpp>
#include <zi/concurrency.hpp>
#include <zi/time.hpp>

#include <map>
#include <set>
#include <list>

namespace zi {
namespace mesh {

class interactive_mesh: non_copyable
{
private:
    enum state
    {
        RUNNING  = 0,
        CLEARING = 1,
        STOPPING = 2
    };

private:
    struct store_task: non_copyable
    {
        vec3u coor_;
        vec3u from_;
        vec3u to_;
        chunk_type_ptr d_;
        mask_type_ptr m_;

        store_task( vec3u coor,
                    vec3u from,
                    vec3u to,
                    chunk_type_ptr d,
                    mask_type_ptr m = mask_type_ptr() )
            : coor_(coor)
            , from_(from)
            , to_(to)
            , d_(d)
            , m_(m)
        { }
    };

private:
    const uint32_t  id_      ;
    smesh_io        smesh_io_;
    fmesh_io        fmesh_io_;
    chunk_io        chunk_io_;


    // The main data mutex. The mutex must be locked whenever any of the members
    // are modified. Note that we are trying to minimize the amount of time
    // spent under locked data mutex.

    zi::mutex                                  mutex_;


    // The data access mutex for the class. Any processing that is happening
    // in parallel should obtain a shared lock, and any process that has to
    // happen serially should obtain an exclusive lock. Used for the clear()
    // method and for zapping the interactive mesh.

    zi::rwmutex                                in_call_mutex_;


    // The update mutex is used to serialize the update requests, and to guard
    // the update_chunks_ - the set of tasks to be completed in the first stage.

    zi::condition_variable                     update_chunks_cv_;
    std::map<vec3u, std::list<store_task*> >   update_chunks_   ;


    // The following monitors guard the update_mip_level_[] sets of tasks to be
    // completed in the next stages. They are also used to trigger the next
    // stage in the pipeline.

    zi::array<zi::condition_variable,9>        update_mip_level_cv_;
    zi::array<std::set<vec3u>,9>               update_mip_level_   ;


    // The following mutexes ensure that the source data is not modified while
    // computing the meshes for a given mip level. When a mip level N is
    // generated we need a write lock to the level N and a read lock to the
    // mip level N-1, as we don't want it's state to change during computation.
    // The special case is when we are generating mip level 0, in which case
    // we don't want the chunks to get modified. For that we acquire a read lock
    // for the chunk_updates_m_ mutex. We also need to acquire write lock
    // for it while in the stage of updating chunks.

    zi::rwmutex                                chunks_mutex_   ;
    zi::array<zi::rwmutex,9>                   mip_level_mutex_;


    // The threads for the stages of the pipeline

    zi::thread                                 update_chunks_stage_thread_    ;
    zi::array<zi::thread,9>                    update_mip_level_stage_threads_;


    // Used to nicely close the interactive_mesh, make sure that all the
    // current processing is done and dumped to the disk, also used for clearing
    // the meshes, by forcing the threads to stop processing.

    state                                       state_         ;
    std::size_t                                 stopping_stage_;

public:
    explicit interactive_mesh( uint32_t id )
        : id_(id)
        , smesh_io_(id)
        , fmesh_io_(id)
        , chunk_io_(id)
        , mutex_()
        , in_call_mutex_()
        , update_chunks_cv_()
        , update_chunks_()
        , update_mip_level_cv_()
        , update_mip_level_()
        , chunks_mutex_()
        , mip_level_mutex_()
        , update_chunks_stage_thread_()
        , update_mip_level_stage_threads_()
        , state_(RUNNING)
        , stopping_stage_(0)
    {
        start_threads();

        // If we decide to be OK with a few minutes of startup time
        // we can remesh all the meshes on load!

        // remesh();
    }

    ~interactive_mesh()
    {
        // We need an exclusive lock for the ~interactive_mesh() method.
        // All other calls must finish.

        zi::rwmutex::write_guard wg(in_call_mutex_);

        stop_stages();

        LOG(debug) << "   Interactive handler for cell " << id_ << " closed";
    }


private:

    // Create and start the stage threads. There are 10 total - update chunks
    // and 9 update mip level stages.

    void start_threads()
    {
        update_chunks_stage_thread_ = zi::thread(
            zi::run_fn(zi::bind(&interactive_mesh::update_chunks_stage,this)));
        update_chunks_stage_thread_.start();

        for ( uint32_t i = 0; i < 9; ++i )
        {
            update_mip_level_stage_threads_[i] = zi::thread(
                zi::run_fn(
                    zi::bind(&interactive_mesh::update_mip_level_stage,
                             this,i)));
            update_mip_level_stage_threads_[i].start();
        }
    }


    // Wait for the stage threads to finish. This is used for either clearing
    // the data or on return.

    void join_threads()
    {
        update_chunks_stage_thread_.join();
        for ( std::size_t i = 0; i < 9; ++i )
        {
            update_mip_level_stage_threads_[i].join();
        }
    }


    // Used to stop the background threads, but first finish all the work
    // that's scheduled to be done. Note there's no lock here, should be
    // called under acquired write lock for in_call_mutex_

    void stop_stages()
    {
        {
            // Set the state to STOPPING and notify the first stage
            zi::mutex::guard g(mutex_);
            state_ = STOPPING;
            update_chunks_cv_.notify_one();
        }

        join_threads();

        {
            zi::mutex::guard g(mutex_);

            // check whether everything is actually done!
            for ( std::size_t i = 0; i < 10; ++i )
            {
                ASSERT_ERROR(update_mip_level_[i].size()==0);
            }

            ASSERT_ERROR(update_chunks_.size()==0);
        }
    }

public:
    state get_state() const
    {
        zi::mutex::guard g(mutex_);
        return state_;
    }

    void set_state( state s )
    {
        zi::mutex::guard g(mutex_);
        state_ = s;
    }

private:

    // Update chunk data with the given list of updates in the order of arrival.
    // If there is any change schedule remeshing the mip level 0 for the given
    // chunk and the chinks that might be affected by the updates

    void
    chunk_update_task( vec3u chunk_coord, const std::list<store_task*>& tasks )
    {
        if ( tasks.size() == 0 )
        {
            return;
        }

        chunk_type_ptr chunk = chunk_io_.read(chunk_coord, true);

        bool do_it = false;

        // We have two different strategies for scheduling chunk remesh
        // updates. When the mask is given, we have to iterate over the
        // whole volume and therefore can extract which adjacent chunks
        // have to be updated. We schedule only ones that have chantes
        // in the raw data. If the mask is not given we simply update
        // all surrounding chunks.

        bool modified[2][2][2] = {{{0,0},{0,0}},{{0,0},{0,0}}};

        FOR_EACH( it, tasks )
        {
            vec3u coor = (*it)->coor_;
            vec3u from = (*it)->from_;
            vec3u to   = (*it)->to_  ;

            chunk_type_ptr d = (*it)->d_;
            mask_type_ptr  m = (*it)->m_;

            uint32_t x0 = coor[0];
            uint32_t y0 = coor[1];
            uint32_t z0 = coor[2];

            uint32_t fx = from[0];
            uint32_t fy = from[1];
            uint32_t fz = from[2];

            uint32_t tx = to[0];
            uint32_t ty = to[1];
            uint32_t tz = to[2];

            ASSERT_ERROR(fx/CHUNK_SIZE==tx/CHUNK_SIZE);
            ASSERT_ERROR(fy/CHUNK_SIZE==ty/CHUNK_SIZE);
            ASSERT_ERROR(fz/CHUNK_SIZE==tz/CHUNK_SIZE);


            //! Deprecated, to be removed. A case with no mask should be handled
            //! in a different manner.
            if ( !m )
            {
                if ( chunk->operator[](indices
                                       [range(fz%CHUNK_SIZE,tz%CHUNK_SIZE+1)]
                                       [range(fy%CHUNK_SIZE,ty%CHUNK_SIZE+1)]
                                       [range(fx%CHUNK_SIZE,tx%CHUNK_SIZE+1)])
                     != d->operator[](indices
                                      [range(fz-z0,tz-z0+1)]
                                      [range(fy-y0,ty-y0+1)]
                                      [range(fx-x0,tx-x0+1)]) )
                {
                    chunk->operator[](indices
                                      [range(fz%CHUNK_SIZE,tz%CHUNK_SIZE+1)]
                                      [range(fy%CHUNK_SIZE,ty%CHUNK_SIZE+1)]
                                      [range(fx%CHUNK_SIZE,tx%CHUNK_SIZE+1)])
                        = d->operator[](indices
                                        [range(fz-z0,tz-z0+1)]
                                        [range(fy-y0,ty-y0+1)]
                                        [range(fx-x0,tx-x0+1)]);

                    do_it = true;
                    for ( uint32_t x = 0; x < 2; ++x )
                    {
                        for ( uint32_t y = 0; y < 2; ++y )
                        {
                            for ( uint32_t z = 0; z < 2; ++z )
                            {
                                modified[x][y][z] = true;
                            }
                        }
                    }
                }
            }
            else
            {
                for ( uint32_t oz = fz-z0, dz = fz%CHUNK_SIZE;
                      oz <= tz-z0; ++oz, ++dz )
                {
                    for ( uint32_t oy = fy-y0, dy = fy%CHUNK_SIZE;
                          oy <= ty-y0; ++oy, ++dy )
                    {
                        for ( uint32_t ox = fx-x0, dx = fx%CHUNK_SIZE;
                              ox <= tx-x0; ++ox, ++dx )
                        {
                            if ( (*m)[oz][oy][ox] &&
                                 ( (*d)[oz][oy][ox] != (*chunk)[dz][dy][dx] ) )
                            {
                                modified[dx==0][dy==0][dz==0] = do_it = true;
                                (*chunk)[dz][dy][dx] = (*d)[oz][oy][ox];
                            }
                        }
                    }
                }
            }

            delete *it;
        }

        if ( do_it )
        {
            chunk_io_.write(chunk, chunk_coord);

            vec3u& c = chunk_coord;

            uint32_t fx = ( c[0] > 0 ) ? c[0] - 1 : c[0];
            uint32_t fy = ( c[1] > 0 ) ? c[1] - 1 : c[1];
            uint32_t fz = ( c[2] > 0 ) ? c[2] - 1 : c[2];

            zi::mutex::guard g(mutex_);

            for ( uint32_t x = fx; x <= c[0]; ++x )
            {
                for ( uint32_t y = fy; y <= c[1]; ++y )
                {
                    for ( uint32_t z = fz; z <= c[2]; ++z )
                    {
                        if ( modified[c[0]-x][c[1]-y][c[2]-z] )
                        {
                            update_mip_level_[0].insert(vec3u(x,y,z));
                        }
                    }
                }
            }
        }
    }


    void update_mip0_task( const vec3u& c )
    {
        chunk_type_ptr chunk = chunk_io_.read_extended(c);

        zi::mesh::marching_cubes<int32_t> mc;
        mc.marche(reinterpret_cast<int32_t*>(chunk->data()),
                  (CHUNK_SIZE+1), (CHUNK_SIZE+1), (CHUNK_SIZE+1));

        vec4u mcoord( c[0], c[1], c[2], 0);
        vec3u next(c[0]/2, c[1]/2, c[2]/2);

        if ( mc.count(id_) )
        {
            int_mesh im;
            im.add(mc.get_triangles(id_));

            simplifier<double> s;
            im.fill_simplifier<double>(s);

            s.prepare();
            s.optimize(s.face_count()/10, 1e-12 );

            std::vector< vec3d > points ;
            std::vector< vec3d > normals;
            std::vector< vec3u > faces  ;

            s.get_faces(points, normals, faces);
            mesh_type_ptr fmd( new mesh_type );
            fmd->add(points, normals, faces);

            //std::cout << "Saving Chunk Coord: " << mcoord << "\n";

            fmesh_io_.write(fmd, mcoord);
            smesh_io_.write(mcoord, s);
            schedule_remesh(next,1);
        }
        else
        {
            ///std::cout << "Erasing Chunk Coord: " << mcoord << "\n";
            fmesh_io_.remove(mcoord);
            smesh_io_.remove(mcoord);
            schedule_remesh(next,1);
        }
    }

    void update_mipn_task( const vec3u& c, uint32_t mip )
    {
        vec4u mcoord( c[0], c[1], c[2], mip);
        vec3u next(c[0]/2, c[1]/2, c[2]/2);

        vec3d offset = vec3d(CHUNK_SIZE<<mip,CHUNK_SIZE<<mip,CHUNK_SIZE<<mip);
        mesh_type_ptr fmd = fmesh_io_.read_from_lower(mcoord, offset);

        if ( fmd )
        {
            simplifier<double> s;
            fmd->fill_simplifier<double>(s);

            s.prepare();
            s.optimize(s.face_count()/8, (1<<(mip-1)));

            std::vector< vec3d > points ;
            std::vector< vec3d > normals;
            std::vector< vec3u > faces  ;

            s.get_faces(points, normals, faces);
            mesh_type_ptr nfmd( new mesh_type );
            nfmd->add(points, normals, faces);

            //std::cout << "Saving Chunk Coord: " << mcoord << "\n";

            fmesh_io_.write(nfmd, mcoord);
            smesh_io_.write(mcoord, s);
            schedule_remesh(next,mip+1);
        }
        else
        {
            //std::cout << "Erasing Chunk Coord: " << mcoord << "\n";
            fmesh_io_.remove(mcoord);
            smesh_io_.remove(mcoord);
            schedule_remesh(next,mip+1);
        }
    }


private:
    void update_chunks_stage()
    {
        bool done = false;
        while (!done)
        {
            zi::wall_timer t;

            std::map<vec3u, std::list<store_task*> > update_chunks;
            {
                zi::mutex::guard g(mutex_);
                while ( update_chunks_.size() == 0 && state_ == RUNNING )
                {
                    update_chunks_cv_.wait(mutex_);
                }

                t.restart();

                // Get the list of the queued chunks to be updated
                // but if we are clearing the datastructure, no need to
                // process the chunks as they'll get deleted anyways.
                if ( state_ != CLEARING )
                {
                    update_chunks.swap(update_chunks_);
                }

                // determine whether the thread should exit after processing
                // the current set of chunks
                done = ( state_ == CLEARING ) || ( state_ == STOPPING );

            }

            // do the actual chunk updates
            if ( update_chunks.size() )
            {
                zi::rwmutex::write_guard g(chunks_mutex_);

                LOG(debug) << "Handler for (" << id_ << ") will update_chunks: "
                           << update_chunks.size();

                {
                    scoped_task_manager tm(16);

                    FOR_EACH( it, update_chunks )
                    {
                        tm.push_back
                            (zi::run_fn(zi::bind
                                        (&interactive_mesh::chunk_update_task,
                                         this, it->first, it->second )));
                    }
                }
            }

            LOG(debug) << "Handler for (" << id_ << ") update_chunks: "
                       << update_chunks.size() << ", " << done
                       << " [" << t.elapsed<double>() << ']';


            {
                // Decide whether to exit the thread based on the previous
                // state (not the current one). The current state will be
                // acknowledged in the next iteration of the loop.
                zi::mutex::guard g(mutex_);

                if ( done )
                {
                    ASSERT_ERROR(stopping_stage_==0);
                    stopping_stage_ = 1;
                }

                // If we have some data ready for the next stage, or we are
                // exiting the thread, notify the next stage
                if ( update_chunks.size() || done )
                {
                    update_mip_level_cv_[0].notify_one();
                }
            }
        }
    }

    void update_mip_level_stage( uint32_t mip )
    {
        bool done = false;
        while (!done)
        {
            zi::wall_timer t;

            std::set<vec3u> to_remesh;
            {
                zi::mutex::guard g(mutex_);
                while ( update_mip_level_[mip].size()==0 && state_==RUNNING )
                {
                    update_mip_level_cv_[mip].wait(mutex_);
                }

                t.reset();


                // If we are in stopping phase we should just wait for the
                // previous stage to be fully completed.
                while ( stopping_stage_ != mip+1 && state_ != RUNNING )
                {
                    update_mip_level_cv_[mip].wait(mutex_);
                }


                // Again, get the list of queued updates unless we are in the
                // clearing phase.
                if ( state_ != CLEARING )
                {
                    to_remesh.swap(update_mip_level_[mip]);
                }


                // Determine whether we are done. If we are done then we should
                // have waited for the right stage before continuing.
                done = ( state_ == CLEARING || state_ == STOPPING );

                if ( done )
                {
                    ASSERT_ERROR(stopping_stage_==mip+1);
                }
            }

            // Do remeshing if needed
            if ( to_remesh.size() )
            {
                // Get the appropriate locks to guard the data
                zi::rwmutex& reading_mutex =
                    ((mip==0)?chunks_mutex_:mip_level_mutex_[mip-1]);

                zi::rwmutex::read_guard  rg(reading_mutex);
                zi::rwmutex::write_guard wg(mip_level_mutex_[mip]);

                LOG(debug) << "Handler for (" << id_ << ") will update_mip: "
                           << mip << ", " << to_remesh.size();

                {
                    scoped_task_manager tm(16);

                    // the processing is different for the zero mip level
                    if ( mip == 0 )
                    {
                        FOR_EACH( it, to_remesh )
                        {
                            tm.insert(zi::run_fn
                                      (zi::bind
                                       (&interactive_mesh::update_mip0_task,
                                        this, *it ) ) );
                        }
                    }
                    else
                    {
                        FOR_EACH( it, to_remesh )
                        {
                            tm.insert(zi::run_fn
                                  (zi::bind
                                   (&interactive_mesh::update_mipn_task,
                                    this, *it, mip ) ) );
                        }
                    }
                }
            }

            LOG(debug) << "Handler for (" << id_ << ") update_mip " << mip
                       << ": " << to_remesh.size() << ", " << done
                       << " [" << t.elapsed<double>() << ']';

            {
                // Needs to decide whether we are actually done.
                zi::mutex::guard g(mutex_);

                if ( done )
                {
                    ASSERT_ERROR(stopping_stage_==mip+1);
                    stopping_stage_=mip+2;
                }

                if ( mip < 8 )
                {
                    if ( to_remesh.size() || done )
                    {
                        update_mip_level_cv_[mip+1].notify_one();
                    }
                }
                else
                {
                    // We might want to notify some cv waiting on the last stage
                    // to be done here... but no need for that with the current
                    // design.
                }
            }
        }
    }

private:
    void schedule_remesh( const vec3u& c, uint32_t mip )
    {
        if ( mip >= 9 )
        {
            return;
        }

        zi::mutex::guard g(mutex_);
        update_mip_level_[mip].insert(c);
    }

    void schedule_remesh_around( const vec3u& c )
    {
        uint32_t fx = ( c[0] > 0 ) ? c[0] - 1 : c[0];
        uint32_t fy = ( c[1] > 0 ) ? c[1] - 1 : c[1];
        uint32_t fz = ( c[2] > 0 ) ? c[2] - 1 : c[2];

        zi::mutex::guard g(mutex_);

        for ( uint32_t x = fx; x <= c[0]; ++x )
        {
            for ( uint32_t y = fy; y <= c[1]; ++y )
            {
                for ( uint32_t z = fz; z <= c[2]; ++z )
                {
                    update_mip_level_[0].insert(vec3u(x,y,z));
                }
            }
        }
    }

    void volume_update_internal(uint32_t x, uint32_t y, uint32_t z,
                                uint32_t w, uint32_t h, uint32_t d,
                                chunk_type_ptr c,
                                mask_type_ptr m = mask_type_ptr() )
    {
        uint32_t x0 = x;
        uint32_t y0 = y;
        uint32_t z0 = z;
        uint32_t x1 = x + w - 1;
        uint32_t y1 = y + h - 1;
        uint32_t z1 = z + d - 1;

        uint32_t mx0 = x0 / CHUNK_SIZE;
        uint32_t my0 = y0 / CHUNK_SIZE;
        uint32_t mz0 = z0 / CHUNK_SIZE;
        uint32_t mx1 = x1 / CHUNK_SIZE;
        uint32_t my1 = y1 / CHUNK_SIZE;
        uint32_t mz1 = z1 / CHUNK_SIZE;

        std::list< std::pair<vec3u,store_task*> > to_add;

        for ( uint32_t i = mx0; i <= mx1; ++i )
        {
            for ( uint32_t j = my0; j <= my1; ++j )
            {
                for ( uint32_t k = mz0; k <= mz1; ++k )
                {
                    uint32_t fromx = std::max( x0, i * CHUNK_SIZE );
                    uint32_t tox   = std::min( x1, (i+1) * CHUNK_SIZE - 1 );

                    uint32_t fromy = std::max( y0, j * CHUNK_SIZE );
                    uint32_t toy   = std::min( y1, (j+1) * CHUNK_SIZE - 1 );

                    uint32_t fromz = std::max( z0, k * CHUNK_SIZE );
                    uint32_t toz   = std::min( z1, (k+1) * CHUNK_SIZE - 1 );

                    vec3u chunk_coord
                        (fromx/CHUNK_SIZE,fromy/CHUNK_SIZE,fromz/CHUNK_SIZE);

                    store_task* task = new store_task(
                        vec3u(x,y,z), vec3u(fromx,fromy,fromz),
                        vec3u(tox,toy,toz), c, m );

                    to_add.push_back(std::make_pair(chunk_coord, task));
                }
            }
        }

        {
            zi::mutex::guard g(mutex_);
            FOR_EACH( it, to_add )
            {
                update_chunks_[it->first].push_back(it->second);
            }
            update_chunks_cv_.notify_one();
        }
    }


public:
    // void volume_update(uint32_t x, uint32_t y, uint32_t z,
    //                    uint32_t w, uint32_t h, uint32_t d,
    //                    const char* data)
    // {
    //     zi::rwmutex::read_guard g(mutex_); // YES this is read guard
    // (reading the dir structure)

    //     chunk_ref_ptr c( new chunk_type_ref(reinterpret_cast<const uint32_t*>
    // (data),
    //                                         extents[d][h][w]) );

    //     {
    //         zi::rwmutex::write_guard g1(chunk_updates_m_);
    //         volume_update_internal(x, y, z, w, h, d, c);
    //     }

    //     //process_up();
    // }

    void chunk_update( uint32_t x, uint32_t y, uint32_t z,
                       const char* data, const char* mask = 0 )
    {
        zi::rwmutex::read_guard g(in_call_mutex_);

        chunk_ref_ptr c( new chunk_type_ref(
                             reinterpret_cast<const uint32_t*>(data),
                             extents[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]) );

        {
            //zi::rwmutex::write_guard g1(chunk_updates_m_);
            //chunk_update_internal( vec3u(x,y,z), c );
        }

        //process_up();
    }

    void volume_update_inner(uint32_t x, uint32_t y, uint32_t z,
                             uint32_t w, uint32_t h, uint32_t d,
                             uint32_t border_width, const char* data,
                             const char* mask = 0)
    {
        zi::rwmutex::read_guard g(in_call_mutex_);


        uint32_t ow = border_width;
        uint32_t dw = 2*ow;

        chunk_type_ptr c(new chunk_type(extents[d-dw][h-dw][w-dw]));

        chunk_ref_ptr corig( new chunk_type_ref(
                                 reinterpret_cast<const uint32_t*>(data),
                                 extents[d][h][w]) );

        c->operator[](indices[range(0,d-dw)][range(0,h-dw)][range(0,w-dw)])
            = corig->operator[](indices[range(ow,d-ow)]
                                [range(ow,h-ow)][range(ow,w-ow)]);

        mask_type_ptr m;

        if ( mask )
        {
            m = mask_type_ptr( new mask_type(extents[w-dw][h-dw][d-dw]) );
            mask_type_ref morig( mask, extents[d][h][w] );

            m->operator[](indices[range(0,d-dw)][range(0,h-dw)][range(0,w-dw)])
                = morig[indices[range(ow,d-ow)][range(ow,h-ow)]
                        [range(ow,w-ow)]];
        }

        {
            // zi::rwmutex::write_guard g(chunk_updates_m_);
            // std::cout << "Chunk Data Update!" << std::endl;

            volume_update_internal(x+ow, y+ow, z+ow, w-dw, h-dw, d-dw, c, m);
        }

        //process_up();
    }

    std::size_t get_hash( const vec4u& c )
    {
        zi::rwmutex::read_guard g(in_call_mutex_);
        return smesh_io_.get_hash(c);
    }

    std::size_t get_hash( uint32_t x, uint32_t y, uint32_t z, uint32_t mip )
    {
        return get_hash( vec4u(x,y,z,mip) );
    }

    std::size_t get_mesh( const vec4u& c, std::string& ret )
    {
        zi::rwmutex::read_guard g(in_call_mutex_);
        return smesh_io_.read(c, ret);
    }

    void get_hashes( std::vector<int64_t>& r, const std::vector<vec4u>& c )
    {
        zi::rwmutex::read_guard g(in_call_mutex_);
        r.resize(c.size());

        for ( std::size_t i = 0; i < c.size(); ++i )
        {
            r[i] = static_cast<int64_t>(smesh_io_.get_hash(c[i]));
        }
    }

    std::size_t get_mesh_if_different( const vec4u& c, std::string& ret,
                                       std::size_t h )
    {
        zi::rwmutex::read_guard g(in_call_mutex_);
        return smesh_io_.read_if_different(c, ret, h);
    }

    void clear()
    {
        // We need an exclusive lock for the clear() method.
        // All other calls must finish, or wait

        zi::rwmutex::write_guard wg(in_call_mutex_);

        {
            // Set the state to CLEARING and notify the first stage
            zi::mutex::guard g(mutex_);
            state_ = CLEARING;
            update_chunks_cv_.notify_one();
        }

        join_threads();

        // Erase all the data and clear the cache
        smesh_io_.erase_all();
        fmesh_io_.erase_all();
        chunk_io_.erase_all();

        {
            zi::mutex::guard g(mutex_);

            // Erase the pending tasks for all the stages

            for ( std::size_t i = 0; i < 10; ++i )
            {
                update_mip_level_[i].clear();
            }

            FOR_EACH( i, update_chunks_ )
            {
                FOR_EACH( j, i->second )
                {
                    delete *j;
                }
            }

            update_chunks_.clear();

            ASSERT_ERROR(stopping_stage_==10);
            state_ = RUNNING;
        }

        start_threads();
    }

    void remesh()
    {
        zi::rwmutex::write_guard wg(in_call_mutex_);
        std::list<vec3u> to_update;

        chunk_io_.get_all(to_update);

        // For the case when we have problems with the file system it might be
        // useful to have the erase_all calls made as well. In that case we
        // might want to sync them with the other reads/writes.

        // smesh_io_.erase_all();
        // fmesh_io_.erase_all();

        {
            zi::mutex::guard g(mutex_);
            FOR_EACH( it, to_update )
            {
                update_mip_level_[0].insert(*it);
            }
            if ( to_update.size() )
            {
                LOG(debug) << "Fully remeshing cell: " << id_
                           << " total: " << to_update.size();

                update_mip_level_cv_[0].notify_one();
            }
        }
    }


}; // class interactive_mesh

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_INTERACTIVE_MESH_HPP
