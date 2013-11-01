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

#ifndef ZI_MESHING_CHUNK_IO_HPP
#define ZI_MESHING_CHUNK_IO_HPP

#include "types.hpp"
#include "files.hpp"
#include "detail/log.hpp"

#include <zi/utility/non_copyable.hpp>
#include <zi/utility/for_each.hpp>
#include <zi/concurrency.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <zlib.h>

namespace zi {
namespace mesh {

class compressed_chunk: non_copyable
{
public:
    zi::rwmutex m_;
    void*       d_;
    uLongf      s_;
    bool        l_;
    bool        e_;

    explicit compressed_chunk(bool e = false)
        : m_()
        , d_(0)
        , s_(0)
        , l_(false)
        , e_(e)
    { }

    ~compressed_chunk()
    {
        zi::rwmutex::write_guard g(m_);
        if ( s_ )
        {
            free(d_);
        }
    }
};

class chunk_io: non_copyable
{
private:
    uint32_t    id_    ;
    std::string prefix_;

    std::map<vec3u, compressed_chunk*>  map_;
    zi::mutex                           m_  ;

    std::map<compressed_chunk*, vec3u> writeback_  ;
    zi::mutex                          writeback_m_;
    zi::thread                         writeback_t_;
    bool                               done_       ;
    std::size_t                        total_      ;

private:
    void writeback_loop()
    {
        bool done = false;
        while (!done)
        {
            zi::this_thread::usleep(500000);

            std::map<compressed_chunk*, vec3u> m;
            {
                {
                    zi::mutex::guard g(writeback_m_);
                    std::swap(m, writeback_);
                    done = done_;
                }

                FOR_EACH( it, m )
                {
                    zi::rwmutex::read_guard g(it->first->m_);

                    if ( !file_io.create_path<3>(prefix_, it->second) )
                    {
                        LOG(error) << "Can't create dir";
                        continue;
                    }

                    vec3u c = it->second;
                    std::string fname =
                        boost::str(boost::format("%s/%d/%d/%d/chunk.raw")
                                   % prefix_ % c[0] % c[1] % c[2] );


                    std::FILE* fp = std::fopen(fname.c_str(), "wb");

                    if ( it->first->s_ != std::fwrite(it->first->d_, 1,
                                                      it->first->s_, fp) )
                    {
                        std::cout << "Dump to disk error\n";
                        exit(-1);
                    }

                    std::fclose(fp);
                }

                if ( m.size() )
                {
                    std::size_t total = 0;
                    {
                        zi::mutex::guard g(m_);
                        total = total_;
                    }
                    LOG(debug) << "Writeback (" << id_ << ") wrote: "
                               << m.size() << " files, total: "
                               << ( total_ / 1024 ) << " KB";
                }
            }
        }
    }

private:
    void load()
    {
        LOG(out) << "Loading chunks for Cell " << id_;

        std::list<std::string> l;
        file::find_files(l, prefix_);

        LOG(out) << "   Found: " << l.size() << " files";

        std::string format = prefix_ + "/%d/%d/%d/chunk.raw";

        std::size_t loaded = 0;
        //std::size_t totals = 0;

        FOR_EACH( it, l )
        {
            int x, y, z;
            if ( ::sscanf(it->c_str(), format.c_str(), &x, &y, &z ) == 3 )
            {
                ++loaded;

                compressed_chunk* cc = get( vec3u(x,y,z) );
                cc->e_ = true;
                //totals += load_from_file( cc, *it );
            }
        }

        //LOG(out) << "   Loaded: " << loaded << " files (" << ( totals/1024) << " KB)\n";
    }

private:

    std::size_t load_from_file( compressed_chunk* cc, const std::string& f )
    {
        if ( cc->l_ )
        {
            return cc->s_;
        }

        std::FILE *fp = std::fopen(f.c_str(), "rb");
        std::fseek(fp, 0, SEEK_END);

        cc->s_ = std::ftell(fp);
        cc->d_ = malloc(cc->s_);

        std::rewind(fp);
        if ( cc->s_ != std::fread(cc->d_, 1, cc->s_, fp) )
        {
            LOG(error) << "Can't read whole file";
        }
        std::fclose(fp);

        return cc->s_;
    }

    compressed_chunk* get( const vec3u& v )
    {
        zi::mutex::guard g(m_);

        compressed_chunk* ret = map_[v];

        if ( ret == 0 )
        {
            ret = new compressed_chunk;
            map_[v] = ret;
        }

        return ret;
    }

    void clear_all()
    {
        {
            zi::mutex::guard g(writeback_m_);
            done_ = 1;
        }

        writeback_t_.join();
        writeback_.clear();
        done_ = 0;

        zi::mutex::guard g(m_);

        FOR_EACH( it, map_ )
        {
            delete it->second;
        }

        file_io.remove_dir(prefix_);

        map_.clear();

        writeback_t_ = zi::thread(zi::run_fn(zi::bind(&chunk_io::writeback_loop,
                                                      this)));
        writeback_t_.start();
    }


public:
    chunk_io( uint32_t id )
        : id_(id)
        , prefix_()
        , map_()
        , m_()
        , writeback_()
        , writeback_m_()
        , writeback_t_(zi::run_fn(zi::bind(&chunk_io::writeback_loop, this)))
        , done_(0)
        , total_(0)
    {
        file_io.create_dir("./data/" + boost::lexical_cast<std::string>(id));
        prefix_ = "./data/" + boost::lexical_cast<std::string>(id) + "/raw";
        load();
        writeback_t_.start();
    }

    ~chunk_io()
    {
        {
            zi::mutex::guard g(writeback_m_);
            done_ = 1;
        }
        writeback_t_.join();
        LOG(out) << "      chunk_io for " << id_ << " died";
    }

private:
    chunk_type_ptr
    read_from_compressed_chunk( compressed_chunk* cc,
                                bool fill_zeros ) const
    {
        if ( cc->d_ == 0 )
        {
            return fill_zeros
                ? chunk_type_ptr(
                    new chunk_type(extents[CHUNK_SIZE]
                               [CHUNK_SIZE][CHUNK_SIZE]))
                :  chunk_type_ptr();
        }

        uLongf out_len = CHUNK_DATA_SIZE*4;
        chunk_type_ptr ret
            ( new chunk_type(extents[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]) );

        int r = ::uncompress( reinterpret_cast<Bytef*>(ret->data()), &out_len,
                              reinterpret_cast<const Bytef*>(cc->d_), cc->s_);
        if ( r != Z_OK )
        {
            std::cout << "Uncompress Error: " << r << "\n";
            exit(-1);
        }

        if ( out_len != CHUNK_DATA_SIZE*4 )
        {
            std::cout << "Uncompress Size Error\n";
            exit(-1);
        }

        return ret;
    }

public:

    chunk_type_ptr read(const vec3u& c, bool fill_zeros = false)
    {
        compressed_chunk* cc = get(c);

        while (1)
        {
            {
                zi::rwmutex::read_guard g(cc->m_);

                // If the chunk is loaded or the chunk file doesn't exist
                // then we extract the data
                if ( cc->l_ || !cc->e_ )
                {
                    return read_from_compressed_chunk(cc, fill_zeros);
                }
            }
            {
                zi::rwmutex::write_guard g(cc->m_);
                ZI_ASSERT(cc->e_);

                // Check whether it's still the case that the chunk
                // is not loaded. Other thread might have done the job
                if ( !cc->l_ && cc->e_ )
                {
                    std::string fname = boost::str(
                        boost::format("%s/%d/%d/%d/chunk.raw")
                        % prefix_ % c[0] % c[1] % c[2] );
                    std::size_t s = load_from_file( cc, fname );
                    cc->l_ = true;

                    {
                        zi::mutex::guard g(m_);
                        total_ += s;
                    }
                }
            }
        }
    }

    bool write(chunk_type_ptr v, const vec3u& c)
    {
        compressed_chunk* cc = get(c);

        zi::rwmutex::write_guard g(cc->m_);

        if ( cc->s_ )
        {
            free(cc->d_);
            {
                zi::mutex::guard g(m_);
                total_ -= cc->s_;
            }
        }

        cc->e_ = cc->l_ = true;
        cc->s_ = compressBound(static_cast<uLongf>(CHUNK_DATA_SIZE*4));
        cc->d_ = malloc(cc->s_);

        int r = ::compress2(reinterpret_cast<Bytef*>(cc->d_), &cc->s_,
                            reinterpret_cast<const Bytef*>(v->data()),
                            CHUNK_DATA_SIZE*4, Z_BEST_COMPRESSION );

        if ( r != Z_OK )
        {
            std::cout << "Compress Error: " << r << "\n";
            exit(-1);
        }

        cc->d_ = realloc(cc->d_, cc->s_);

        {
            zi::mutex::guard g(writeback_m_);
            writeback_[cc] = c;
        }

        {
            zi::mutex::guard g(m_);
            total_ += cc->s_;
        }

        return true;
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

    void merge(chunk_type& v, const chunk_type& x, side_to_attach s)
    {
        static const std::size_t N = CHUNK_SIZE;

        //std::cout << "Overlap: " << (int)(s) << "\n";

        switch (s)
        {
        case z_side:
            v[indices[N][range(0,N)][range(0,N)]] =
                x[indices[0][range(0,N)][range(0,N)]];
            break;
        case y_side:
            v[indices[range(0,N)][N][range(0,N)]] =
                x[indices[range(0,N)][0][range(0,N)]];
            break;
        case x_side:
            v[indices[range(0,N)][range(0,N)][N]] =
                x[indices[range(0,N)][range(0,N)][0]];
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

    void merge(chunk_type_ptr v, chunk_type_ptr x, side_to_attach s)
    {
        merge(*v.get(), *x.get(), s);
    }

public:
    chunk_type_ptr read_extended(const vec3u& c,
                                 chunk_type_ptr v = chunk_type_ptr() )
    {
        static const std::size_t N = CHUNK_SIZE+1;

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

    void erase_all()
    {
        clear_all();
    }

    void get_all( std::list<vec3u>& r )
    {
        zi::mutex::guard g(m_);
        FOR_EACH( it, map_ )
        {
            r.push_back(it->first);
        }
    }

}; // class chunk_io


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_CHUNK_IO_HPP
