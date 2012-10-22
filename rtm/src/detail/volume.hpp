#ifndef ZI_MESHING_DETAIL_VOLUME_HPP
#define ZI_MESHING_DETAIL_VOLUME_HPP

#include <boost/multi_array.hpp>
#include <boost/multi_array/types.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <zi/vl/vl.hpp>
#include <zi/mesh/int_mesh.hpp>

#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>
//#include "../gzstream.hpp"

namespace zi {
namespace mesh {

namespace {

boost::multi_array_types::extent_gen extents;
boost::multi_array_types::index_gen  indices;

//boost::fortran_storage_order storage_order;

}

namespace vol {
using namespace boost::multi_array_types;

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

typedef zi::vl::vec<uint32_t, 3> chunk_coord;

template< uint32_t N >
inline bool create_path(const std::string& prefix, const zi::vl::vec<uint32_t, N>& v)
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

inline std::string strcoord(const chunk_coord& c)
{
    return boost::str( boost::format("/%d/%d/%d") % c[0] % c[1] % c[2] );
}

namespace {

chunk_coord incx(1,0,0);
chunk_coord incy(0,1,0);
chunk_coord incz(0,0,1);

}

inline std::size_t add_to_int_mesh(int_mesh& m, const std::string& fname,
                                   uint64_t x=0, uint64_t y=0, uint64_t z=0)
{
    std::ifstream in(fname.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {
        struct stat filestatus;
        ::stat(fname.c_str(), &filestatus );
        if ( filestatus.st_size == 0 )
        {
            return 0;
        }

        std::size_t r = filestatus.st_size / sizeof(int_mesh::triangle_t);

        char* buff = new char[filestatus.st_size];

        std::copy( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), buff );

        int_mesh::triangle_t* triangles = reinterpret_cast<int_mesh::triangle_t*>(buff);
        m.add(triangles, r, x, y, z);

        delete [] buff;

        return r;
    }
    else
    {
        return 0;
    }
}

inline bool save_int_mesh(const int_mesh& m, const std::string& fname)
{
    std::ofstream out(fname.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
    if (out)
    {
        int_mesh::triangle_t* buff = new int_mesh::triangle_t[m.size()];
        std::copy( m.data().begin(), m.data().end(), buff );

        out.write(reinterpret_cast<char*>(buff),
                  static_cast<std::streamsize>(sizeof(int_mesh::triangle_t)*m.size()));

        delete [] buff;

        return true;
    }
    else
    {
        return false;
    }
}

inline bool save_int_mesh(const int_mesh& m, const chunk_coord& c, uint32_t id)
{
    create_path<3>("./data/imesh", c);
    std::string s = boost::str( boost::format("./data/imesh/%d/%d/%d/%d.imesh")
                                % c[0] % c[1] % c[2] % id );
    return save_int_mesh(m, s);
}

inline std::size_t add_to_int_mesh(boost::shared_ptr<int_mesh> m, const std::string& fname,
                                   uint64_t x=0, uint64_t y=0, uint64_t z=0)
{
    return add_to_int_mesh(*m.get(), fname, x, y, z);
}

inline bool save_int_mesh(boost::shared_ptr<int_mesh> m, const std::string& fname)
{
    return save_int_mesh(*m.get(), fname);
}

inline bool save_int_mesh(boost::shared_ptr<int_mesh> m, const chunk_coord& c, uint32_t id)
{
    return save_int_mesh(*m.get(), c, id);
}

inline boost::shared_ptr<int_mesh> load_int_mesh( zi::vl::vec<uint32_t, 4> c )
{
    boost::shared_ptr<int_mesh> m(new int_mesh);
    std::string s = boost::str( boost::format("./data/imesh/%d/%d/%d/%d.imesh")
                                % c[0] % c[1] % c[2] % c[3] );
    if ( !add_to_int_mesh(m, s) )
    {
        m.reset();
    }

    return m;
}


template< typename T >
inline std::size_t add_to_face_mesh(face_mesh<T>& m, const std::string& fname,
                                    const T& x=T(0), const T& y=T(0), const T& z=T(0))
{
    std::ifstream in(fname.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {
        struct stat filestatus;
        ::stat(fname.c_str(), &filestatus );
        if ( filestatus.st_size == 0 )
        {
            return 0;
        }

        std::size_t tot = filestatus.st_size;

        char* buff = new char[filestatus.st_size];
        std::copy( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), buff );

        char* curr = buff;

        std::size_t* sizes = reinterpret_cast<std::size_t*>(curr);
        curr += sizeof( std::size_t ) * 2;

        typename face_mesh<T>::point_t* pts = reinterpret_cast<typename face_mesh<T>::point_t*>(curr);
        curr += sizeof( typename face_mesh<T>::point_t) * sizes[0];

        typename face_mesh<T>::point_t* nps = reinterpret_cast<typename face_mesh<T>::point_t*>(curr);
        curr += sizeof( typename face_mesh<T>::point_t ) * sizes[0];

        typename face_mesh<T>::face_t* fcs = reinterpret_cast<typename face_mesh<T>::face_t*>(curr);

        m.add(pts, nps, sizes[0], fcs, sizes[1], x, y, z);

        delete [] buff;

        return tot;
    }
    else
    {
        return 0;
    }
}

template< typename T >
inline bool save_face_mesh(const face_mesh<T>& m, const std::string& fname)
{
    std::ofstream out(fname.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
    if (out)
    {
        std::size_t sizes[2];
        sizes[0] = m.points().size();
        sizes[1] = m.faces().size();

        typename face_mesh<T>::point_t* pbuff = new typename face_mesh<T>::point_t[sizes[0]];
        typename face_mesh<T>::point_t* nbuff = new typename face_mesh<T>::point_t[sizes[0]];
        typename face_mesh<T>::face_t*  fbuff = new typename face_mesh<T>::face_t[sizes[1]];

        std::copy( m.points().begin(), m.points().end(), pbuff );
        std::copy( m.normals().begin(), m.normals().end(), nbuff );
        std::copy( m.faces().begin(), m.faces().end(), fbuff );

        out.write(reinterpret_cast<char*>(sizes),
                  static_cast<std::streamsize>(2*sizeof(std::size_t)));

        out.write(reinterpret_cast<char*>(pbuff),
                  static_cast<std::streamsize>(sizes[0]*sizeof(typename face_mesh<T>::point_t)));

        out.write(reinterpret_cast<char*>(nbuff),
                  static_cast<std::streamsize>(sizes[0]*sizeof(typename face_mesh<T>::point_t)));

        out.write(reinterpret_cast<char*>(fbuff),
                  static_cast<std::streamsize>(sizes[1]*sizeof(typename face_mesh<T>::face_t)));

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

template< typename T >
inline bool save_face_mesh(const face_mesh<T>& m, uint32_t mip, const chunk_coord& c, uint32_t id)
{
    create_path<4>("./data/fmesh", zi::vl::vec<uint32_t,4>(mip, c));
    std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.fmesh")
                                % mip % c[0] % c[1] % c[2] % id);
    return save_face_mesh(m, s);
}


template< typename T >
inline std::size_t add_to_face_mesh(boost::shared_ptr<face_mesh<T> > m, const std::string& fname,
                                    const T& x=T(0), const T& y=T(0), const T& z=T(0))
{
    return add_to_face_mesh<T>(*m.get(), fname, x, y, z);
}

template< typename T >
inline boost::shared_ptr<face_mesh<T> > load_face_mesh( zi::vl::vec<uint32_t, 5> c )
{
    //std::cout << "Load face mesh: " << c << "\n";
    boost::shared_ptr<face_mesh<T> > m(new face_mesh<T> );
    std::string s = boost::str( boost::format("./data/fmesh/%d/%d/%d/%d/%d.fmesh")
                                % c[0] % c[1] % c[2] % c[3] % c[4] );
    if ( !add_to_face_mesh<T>(m, s) )
    {
        //std::cout << "No face mesh: " << c << "\n";
        m.reset();
    }

    return m;
}

template< typename T >
inline boost::shared_ptr<face_mesh<T> >
load_face_mesh_from_lower( zi::vl::vec<uint32_t, 5> c, const zi::vl::vec3d& off )
{
    if ( c[0] == 0 )
    {
        return load_face_mesh<T>( c );
    }

    //std::cout << "Load face mesh from lower: " << c << "\n";

    boost::shared_ptr<face_mesh<T> > m(new face_mesh<T> );

    for ( uint32_t dx = 0; dx < 2; ++dx )
    {
        for ( uint32_t dy = 0; dy < 2; ++dy )
        {
            for ( uint32_t dz = 0; dz < 2; ++dz )
            {
                boost::shared_ptr<face_mesh<T> > x = load_face_mesh<T>
                    ( zi::vl::vec<uint32_t, 5>(c[0]-1,c[1]*2+dx,c[2]*2+dy,c[3]*2+dz,c[4]) );
                m->add(x, off[0]*dx, off[1]*dy, off[2]*dz);
            }
        }
    }

    if ( m->size() == 0 )
    {
        m.reset();
    }

    return m;
}


template< typename T >
inline bool save_face_mesh(boost::shared_ptr<face_mesh<T> > m, const std::string& fname)
{
    return save_face_mesh<T>(*m.get(), fname);
}

template< typename T >
inline bool save_face_mesh(boost::shared_ptr<face_mesh<T> > m, uint32_t mip,
                           const chunk_coord& c, uint32_t id)
{
    return save_face_mesh<T>(*m.get(), mip, c, id);
}


template< typename T, std::size_t N >
inline boost::shared_ptr<boost::multi_array<T,3> > from_file(const std::string& fname, bool gzipped = true)
{
    //std::cout << "Trying to load: " << fname << std::endl;

    if ( gzipped )
    {
        gzFile fin;

        if ( (fin = gzopen(fname.c_str(), "r")) )
        {
            boost::shared_ptr<boost::multi_array<T,3> > ret
                ( new boost::multi_array<T,3>(extents[N][N][N]) );

            std::size_t r = gzread( fin, reinterpret_cast<void*>(ret->data()), sizeof(T)*N*N*N );
            gzclose(fin);

            std::cout << "GZREAD: " << r  << " / " << (sizeof(T)*N*N*N) << "\n";

            if ( r == sizeof(T)*N*N*N )
            {
                return ret;
            }
        }

        return boost::shared_ptr<boost::multi_array<uint32_t,3> >();

        // std::ifstream fin(fname.c_str(), std::ios::in | std::ios::binary);
        // if (fin)
        // {
        //     boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        //     in.push(boost::iostreams::zlib_decompressor());
        //     in.push(fin);

        //     std::string d;
        //     std::ostringstream is;
        //     boost::iostreams::copy(in, is);

        //     is.str(d);

        //     boost::shared_ptr<boost::multi_array<T,3> > ret
        //         ( new boost::multi_array<T,3>(extents[N][N][N]) );

        //     std::copy( d.c_str(), d.c_str() + d.size(),
        //                reinterpret_cast<char*>(ret->data()) );

        //     return ret;
    // }
    // else
    // {
    //     return boost::shared_ptr<boost::multi_array<T,3> >();
    // }
    }

    std::ifstream in(fname.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {

        struct stat filestatus;
        ::stat(fname.c_str(), &filestatus );
        if ( filestatus.st_size != (N*N*N*sizeof(T)) )
        {
            return boost::shared_ptr<boost::multi_array<T,3> >();
        }

        boost::shared_ptr<boost::multi_array<T,3> > ret
            ( new boost::multi_array<T,3>(extents[N][N][N]) );

        std::copy( std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(),
                   reinterpret_cast<char*>(ret->data()) );

        return ret;
    }
    else
    {
        return boost::shared_ptr<boost::multi_array<T,3> >();
    }
}

template< typename T, std::size_t N >
inline void do_merge(boost::multi_array<T,3>& v, const boost::multi_array<T,3>& x, int s)
{
    typedef boost::multi_array<T,3>            array_type;
    typedef typename array_type::index_range   range     ;

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

template< typename T, std::size_t N>
inline void merge(boost::shared_ptr<boost::multi_array<T,3> > v,
                  boost::shared_ptr<boost::multi_array<T,3> > x,
                  int s)
{
    do_merge<T,N>(*v.get(), *x.get(), s);
}

template< typename T, std::size_t N >
inline boost::shared_ptr<boost::multi_array<T,3> > load_volume(
    boost::shared_ptr<boost::multi_array<T,3> > v,
    const std::string& prefix,
    const std::string& suffix,
    const chunk_coord& c)
{
    boost::shared_ptr<boost::multi_array<T,3> > r
        ( new boost::multi_array<T,3>(extents[N][N][N]) );

    if ( v )
    {
        merge<T,N-1>(r, v, no_side);
    }

    chunk_coord nc = c; ++nc[0];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, x_side);

    nc = c; ++nc[1];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, y_side);

    nc = c; ++nc[2];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, z_side);

    nc = c; ++nc[1]; ++nc[2];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, yz_side);

    nc = c; ++nc[0]; ++nc[1];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, xy_side);

    nc = c; ++nc[0]; ++nc[2];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, xz_side);

    nc = c; ++nc[0]; ++nc[2]; ++nc[1];
    v = from_file<T,N-1>(prefix+strcoord(nc)+suffix);
    if ( v ) merge<T,N-1>(r, v, xyz_side);

    return r;
}


template< typename T, std::size_t N >
inline boost::shared_ptr<boost::multi_array<T,3> > load_volume(const std::string& prefix,
                                                               const std::string& suffix,
                                                               const chunk_coord& c)
{
    boost::shared_ptr<boost::multi_array<T,3> > v = from_file<T,N-1>(prefix+strcoord(c)+suffix);

    return load_volume<T,N>(v,prefix,suffix,c);
}

template< typename T, std::size_t N >
inline bool to_file(boost::shared_ptr<boost::multi_array<T,3> > v, const std::string& fname)
{
    std::ofstream out(fname.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
    if (out)
    {
        out.write(reinterpret_cast<char*>(v->data()),
                  static_cast<std::streamsize>(sizeof(T)*N*N*N));
        return true;
    }
    else
    {
        return false;
    }
}

template< typename T, std::size_t N >
inline bool save_volume(boost::shared_ptr<boost::multi_array<T,3> > v, const chunk_coord& c)
{
    create_path<3>("./data/raw", c);
    std::string fname = boost::str( boost::format("./data/raw/%d/%d/%d/chunk.raw") % c[0] % c[1] % c[2] );
    std::ofstream out(fname.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );

    if (out)
    {
        std::ostringstream sout(std::string(reinterpret_cast<char*>(v->data()),
                                            static_cast<std::streamsize>(sizeof(T)*N*N*N)),
                                std::ios::out | std::ios::binary | std::ios::trunc );

        boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
        in.push(sout);
        in.push(boost::iostreams::zlib_compressor());

        boost::iostreams::copy(in, out);

        std::cout << "HERE\n";

        //out.write(reinterpret_cast<char*>(v->data()),
        //static_cast<std::streamsize>(sizeof(T)*N*N*N));
        return true;
    }
    else
    {
        return false;
    }
}



} // namespace vol
} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_VOLUME_HPP
