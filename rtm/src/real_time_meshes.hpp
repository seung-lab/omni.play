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

#ifndef ZI_MESHING_REAL_TIME_MESHES_HPP
#define ZI_MESHING_REAL_TIME_MESHES_HPP

#include "detail/volume.hpp"
#include "detail/rwlock_pool.hpp"
#include "detail/cache.hpp"
#include "mesh_cache.hpp"

namespace zi {
namespace mesh {

typedef zi::vl::vec<uint32_t, 3> vec3u;
typedef zi::vl::vec<uint32_t, 4> vec4u;
typedef zi::vl::vec<uint32_t, 5> vec5u;

typedef boost::shared_ptr<int_mesh>           int_mesh_ptr  ;
typedef boost::multi_array<uint32_t,3>        chunk_type    ;
typedef boost::shared_ptr<chunk_type>         chunk_type_ptr;

typedef boost::shared_ptr<face_mesh<double> > face_mesh_ptr ;

inline bool save_chunk(chunk_type_ptr v, const vec3u& c)
{
    vol::create_path<3>("./data/raw", c);
    std::string fname = boost::str(
        boost::format("./data/raw/%d/%d/%d/chunk.raw") % c[0] % c[1] % c[2] );

    gzFile out;

    if ( ( out = gzopen( fname.c_str(), "w" )) )
    {
        std::size_t r = gzwrite(out, reinterpret_cast<const void*>(v->data()),
                                sizeof(uint32_t)*128*128*128);
        std::cout << "GZWRITE: " << r  << " / " << (sizeof(uint32_t)*128*128*128) << "\n";
        gzclose(out);
        return true;
    }
    else
    {
        return false;
    }

}

inline boost::shared_ptr<boost::multi_array<uint32_t,3> > read_chunk(const vec3u& c)
{
    //std::cout << "Trying to load: " << fname << std::endl;
    std::string fname = boost::str(
        boost::format("./data/raw/%d/%d/%d/chunk.raw") % c[0] % c[1] % c[2] );

    gzFile fin;

    if ( (fin = gzopen(fname.c_str(), "r")) )
    {
        boost::shared_ptr<boost::multi_array<uint32_t,3> > ret
            ( new boost::multi_array<uint32_t,3>(extents[128][128][128]) );

        std::size_t r = gzread( fin, reinterpret_cast<void*>(ret->data()),
                                sizeof(uint32_t)*128*128*128);
        gzclose(fin);

        std::cout << "GZREAD: " << r  << " / " << (sizeof(uint32_t)*128*128*128) << "\n";

        if ( r == sizeof(uint32_t)*128*128*128 )
        {
            return ret;
        }
    }
    return boost::shared_ptr<boost::multi_array<uint32_t,3> >();
}

inline boost::shared_ptr<boost::multi_array<uint32_t,3> > read_chunk_forced(const vec3u& c)
{
    //std::cout << "Trying to load: " << fname << std::endl;
    std::string fname = boost::str(
        boost::format("./data/raw/%d/%d/%d/chunk.raw") % c[0] % c[1] % c[2] );

    gzFile fin;

    boost::shared_ptr<boost::multi_array<uint32_t,3> > ret
        ( new boost::multi_array<uint32_t,3>(extents[128][128][128]) );


    if ( (fin = gzopen(fname.c_str(), "r")) )
    {

        std::size_t r = gzread( fin, reinterpret_cast<void*>(ret->data()),
                                sizeof(uint32_t)*128*128*128);
        gzclose(fin);

        std::cout << "GZREAD: " << r  << " / " << (sizeof(uint32_t)*128*128*128) << "\n";
    }
    return ret;
}

inline std::size_t unique_hashes( std::map<uint32_t, std::size_t>& ret,
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

struct chunk_task
{
private:
    vec3u                            coord_     ;
    chunk_type_ptr                   chunk_     ;

public:
    chunk_task(uint32_t x, uint32_t y, uint32_t z, const uint32_t* data)
        : coord_(x,y,z)
        , chunk_(new chunk_type(extents[128][128][128]))
    {
        std::copy(data, data+128*128*128, chunk_->data());
    }

    chunk_task(uint32_t x, uint32_t y, uint32_t z, chunk_type_ptr c)
        : coord_(x,y,z)
        , chunk_(new chunk_type(extents[128][128][128]))
    {

        typedef boost::multi_array<uint32_t,3>            array_type;
        typedef array_type::index_range                   range;

        chunk_->operator[](indices[range(0,128)][range(0,128)][range(0,128)])
            = c->operator[](indices[range(0,128)][range(0,128)][range(0,128)]);
    }

    chunk_task(const vec3u& c, const uint32_t* data)
        : coord_(c)
        , chunk_(new chunk_type(extents[128][128][128]))
    {
        std::copy(data, data+128*128*128, chunk_->data());
    }

    const vec3u& coord() const
    {
        return coord_;
    }

    chunk_type_ptr chunk() const
    {
        return chunk_;
    }

};

typedef boost::shared_ptr<chunk_task>               chunk_task_ptr  ;

class real_time_meshes
{
private:
    zi::mutex                                      global_mutex_   ;
    boost::shared_ptr< std::list<chunk_task_ptr> > pending_tasks_  ;
    zi::task_manager::simple                       serial_tm_      ;
    zi::task_manager::simple                       remesh_tm_      ;
    std::size_t                                    remeshing_done_ ;
    std::size_t                                    remeshing_pend_ ;
    zi::condition_variable                         remesh_cv_      ;

private:
    rwlock_pool<vec3u, 1024, zi::vl::hash<vec3u> > chunk_locks_     ;
    rwlock_pool<vec4u, 1024, zi::vl::hash<vec4u> > int_mesh_locks_  ;
    rwlock_pool<vec5u, 1024, zi::vl::hash<vec5u> > face_mesh_locks_ ;

    std::map<vec4u,std::size_t>                    hashes_         ;
    zi::mutex                                      hashes_mutex_   ;
    std::set<vec4u>                                hashes_updated_ ;
    zi::mutex                                      meshes_mutex_   ;
    std::set<vec4u>                                meshes_updated_ ;

private:
    cache<face_mesh_cache>                         face_mesh_cache_;

public:
    static const uint32_t max_mip = 8;

public:
    real_time_meshes()
        : global_mutex_()
        , pending_tasks_(new std::list<chunk_task_ptr> )
        , serial_tm_(1)
        , remesh_tm_(32)
        , remeshing_done_(0)
        , remeshing_pend_(0)
        , remesh_cv_()
        , chunk_locks_()
        , int_mesh_locks_()
        , face_mesh_locks_()
        , hashes_()
        , hashes_mutex_()
        , hashes_updated_()
        , meshes_updated_()
        , meshes_mutex_()
        , face_mesh_cache_()
    {
        serial_tm_.start();
    }

    ~real_time_meshes()
    {
        serial_tm_.join();
        remesh_tm_.join();
    }


private:
    face_mesh_ptr do_load_face_mesh(uint32_t mip, vec4u c)
    {
        vec5u ktoget(mip, c);

        face_mesh_locks_.acquire_read(ktoget);
        face_mesh_ptr x = face_mesh_cache_.get(ktoget).get();
        face_mesh_locks_.release_read(ktoget);

        return x;
    }

    void do_save_face_mesh(const face_mesh<double>& fmd, uint32_t mip, vec4u c)
    {
        vec5u ktoset(mip, c);

        face_mesh_locks_.acquire_write(ktoset);
        face_mesh_cache_.erase(ktoset);
        vol::save_face_mesh(fmd, mip, vec3u(c[0],c[1],c[2]),c[3]);
        face_mesh_locks_.release_write(ktoset);
    }


private:
    void do_save_chunk(chunk_task_ptr task)
    {
        std::cout << "Saving Chunk " << task->coord() << " \n";
        chunk_locks_.acquire_write(task->coord());
        save_chunk(task->chunk(), task->coord());
        chunk_locks_.release_write(task->coord());
    }

    void do_process_chunk(vec3u c)
    {
        std::cout << "Processing Chunk " << c << " \n";
        chunk_type_ptr chunk = vol::load_volume<uint32_t,129>("./data/raw", "/chunk.raw", c);

        std::map<uint32_t,std::size_t> hashes;

        std::list<uint32_t> to_int_meshify;

        unique_hashes(hashes, chunk->data(), chunk->num_elements());
        {
            zi::mutex::guard g(hashes_mutex_);
            FOR_EACH( it, hashes )
            {
                std::cout << "Unique found: " << it->first << '\n';

                vec4u vc(it->first,c);
                std::map<vec4u,std::size_t>::iterator ht = hashes_.find(vc);
                if ( ht == hashes_.end() )
                {
                    hashes_[vc] = it->second;
                    hashes_updated_.insert(vc);
                    to_int_meshify.push_back(it->first);
                }
                else
                {
                    if ( ht->second != it->second )
                    {
                        ht->second = it->second;
                        hashes_updated_.insert(vc);
                        to_int_meshify.push_back(it->first);
                    }
                }
            }
        }

        if ( to_int_meshify.size() )
        {
            zi::mesh::marching_cubes<int32_t> mc;
            mc.marche(reinterpret_cast<int32_t*>(chunk->data()), 129, 129, 129);

            FOR_EACH( it, to_int_meshify )
            {
                uint32_t id = *it;
                std::cout << "Updated id " << id << " in the chunk " << c << "\n";
                if ( mc.count(id) )
                {
                    std::cout << "Updated FOR REAL id " << id << " in the chunk " << c << "\n";
                    int_mesh_ptr im(new int_mesh);
                    im->add(mc.get_triangles(id));

                    int_mesh_locks_.acquire_write(vec4u(c,id));
                    vol::save_int_mesh(im, c, id);
                    int_mesh_locks_.release_write(vec4u(c,id));

                    simplifier<double> s;
                    im->fill_simplifier<double>(s);

                    s.prepare();
                    s.optimize(s.face_count()/100, 1e-8 );

                    std::vector< zi::vl::vec3d > points ;
                    std::vector< zi::vl::vec3d > normals;
                    std::vector< vec3u >         faces  ;

                    s.get_faces(points, normals, faces);
                    face_mesh<double> fmd;
                    fmd.add(points, normals, faces);

                    do_save_face_mesh(fmd, 0, vec4u(c,id) );

                    {
                        zi::mutex::guard g(meshes_mutex_);
                        meshes_updated_.insert(vec4u(c/2,id));
                    }
                }
            }
        }
    }

    void do_process_mipn( uint32_t mip, vec4u c )
    {
        std::cout << "Processing MIP: " << mip << " for " << c << "\n";
        vl::vec3d offset = vl::vec3d(128<<mip,128<<mip,128<<mip);

        face_mesh_ptr m(new face_mesh<double>);

        for ( uint32_t dx = 0; dx < 2; ++dx )
        {
            for ( uint32_t dy = 0; dy < 2; ++dy )
            {
                for ( uint32_t dz = 0; dz < 2; ++dz )
                {
                    face_mesh_ptr x = do_load_face_mesh(
                        mip-1,
                        vec4u(c[0]*2+dx, c[1]*2+dy, c[2]*2+dz, c[3]) );
                    m->add(x, offset[0]*dz, offset[1]*dy, offset[2]*dx);
                }
            }
        }

        zi::mesh::simplifier<double> s;
        m->fill_simplifier<double>(s);

        s.prepare(false);
        s.optimize(s.face_count()/8, 0.5*(1<<(mip-1)));

        std::vector< zi::vl::vec3d > points ;
        std::vector< zi::vl::vec3d > normals;
        std::vector< vec3u >         faces  ;

        s.get_faces(points, normals, faces);
        face_mesh<double> fmd;
        fmd.add(points, normals, faces);

        do_save_face_mesh(fmd, mip, c);

        if ( mip < max_mip )
        {
            zi::mutex::guard g(meshes_mutex_);
            meshes_updated_.insert(vec4u(c[0]/2,c[1]/2,c[2]/2,c[3]));
        }
    }

    void remesh_subvolume( uint32_t x0, uint32_t y0, uint32_t z0,
                           uint32_t fx, uint32_t fy, uint32_t fz,
                           uint32_t tx, uint32_t ty, uint32_t tz,
                           boost::multi_array_ref<uint32_t,3> d )
    {

        typedef boost::multi_array<uint32_t,3>            array_type;
        typedef array_type::index_range                   range;

        ZI_ASSERT(fx/128==tx/128);
        ZI_ASSERT(fy/128==ty/128);
        ZI_ASSERT(fz/128==tz/128);

        vec3u chunk_coord(fx / 128, fy / 128, fz / 128 );
        std::map<uint32_t,std::size_t> old_hashes, new_hashes;
        chunk_type_ptr chunk;

        chunk_locks_.acquire_write(chunk_coord);

        // get old chunk and old hashes!
        chunk = read_chunk_forced(chunk_coord);
        unique_hashes(old_hashes, chunk->data(), chunk->num_elements());

        // update the chunk
        chunk->operator[](indices[range(fx%128,tx%128)][range(fy%128,ty%128)][range(fz%128,tz%128)])
            = d[indices[range(fx-x0,tx-x0)][range(fy-y0,ty-y0)][range(fz-z0,tz-z0)]];

        // get new hashes
        unique_hashes(new_hashes, chunk->data(), chunk->num_elements());

        // save the new chunk
        save_chunk(chunk, chunk_coord);

        chunk_locks_.release_write(chunk_coord);
    }

    void do_remesh( boost::shared_ptr< std::list<chunk_task_ptr> > t )
    {
        std::cout << "Do Remesh Starting " << t->size() << " pending tasks\n";
        std::list<chunk_task_ptr>& tasks = *t.get();
        std::set<vec3u> touched;

        remesh_tm_.start();

        FOR_EACH( it, tasks )
        {
            remesh_tm_.push_back(zi::bind(&real_time_meshes::do_save_chunk, this, *it));
            const vec3u& c = (*it)->coord();
            touched.insert(c);

            if ( c[0] > 0 )
            {
                touched.insert(vec3u(c[0]-1,c[1],c[2]));
            }
            if ( c[1] > 0 )
            {
                touched.insert(vec3u(c[0],c[1]-1,c[2]));
            }
            if ( c[2] > 0 )
            {
                touched.insert(vec3u(c[0],c[1],c[2]-1));
            }
        }

        tasks.clear();

        remesh_tm_.join();

        {
            std::set<vec4u>  hashes_updated;
            zi::mutex::guard g(hashes_mutex_);
            hashes_updated.swap(hashes_updated_);
            ZI_ASSERT(hashes_updated.size()==0);
        }

        {
            std::set<vec4u>  meshes_updated;
            zi::mutex::guard g(meshes_mutex_);
            meshes_updated.swap(meshes_updated_);
            ZI_ASSERT(meshes_updated.size()==0);
        }

        remesh_tm_.start();

        FOR_EACH( it, touched )
        {
            remesh_tm_.push_back(zi::bind(&real_time_meshes::do_process_chunk, this, *it));
        }

        remesh_tm_.join();

        for ( uint32_t mip = 1; mip <= max_mip; ++mip )
        {
            std::cout << ">>>Working on MIP: " << mip << "\n";

            std::set<vec4u> meshes_updated;
            {
                zi::mutex::guard g(meshes_mutex_);
                meshes_updated.swap(meshes_updated_);
            }

            remesh_tm_.start();

            FOR_EACH( it, meshes_updated )
            {
                remesh_tm_.push_back(zi::bind(&real_time_meshes::do_process_mipn, this, mip, *it));
            }

            remesh_tm_.join();

            std::cout << ">>>Working on MIP: " << "done" << "\n";
        }

        {
            std::set<vec4u>  hashes_updated;
            zi::mutex::guard g(hashes_mutex_);
            hashes_updated.swap(hashes_updated_);
        }

        std::cout << ">>>>>>>>AAAAALLLLDNONE\n\n\n\n" << std::cout;

        {
            zi::mutex::guard g(global_mutex_);
            ++remeshing_done_;
            remesh_cv_.notify_all();
        }

    }

public:
    void update_chunk(uint32_t x, uint32_t y, uint32_t z, const uint32_t* data)
    {
        chunk_task_ptr t(new chunk_task(x,y,z,data));
        {
            zi::mutex::guard g(global_mutex_);
            pending_tasks_->push_back(t);
            std::cout << "Now Has " << pending_tasks_->size() << " pending tasks\n";
        }
    }

    void update_chunk(uint32_t x, uint32_t y, uint32_t z, chunk_type_ptr c)
    {
        chunk_task_ptr t(new chunk_task(x,y,z,c));
        {
            zi::mutex::guard g(global_mutex_);
            pending_tasks_->push_back(t);
            std::cout << "Now Has " << pending_tasks_->size() << " pending tasks\n";
        }
    }

    void remesh( bool async = true )
    {
        zi::mutex::guard g(global_mutex_);
        std::size_t my_turn = ++remeshing_pend_;

        boost::shared_ptr< std::list<chunk_task_ptr> > pending_tasks
            (new std::list<chunk_task_ptr> );

        pending_tasks.swap(pending_tasks_);
        serial_tm_.push_back(zi::bind(&real_time_meshes::do_remesh, this, pending_tasks));

        if ( !async )
        {
            while ( my_turn > remeshing_done_ )
            {
                remesh_cv_.wait(global_mutex_);
            }
        }
    }


}; // real_time_meshes

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_REAL_TIME_MESHES_HPP
