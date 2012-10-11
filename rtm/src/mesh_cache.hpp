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

#ifndef ZI_MESHING_MESH_CACHE_HPP
#define ZI_MESHING_MESH_CACHE_HPP 1

#include <zi/vl/vl.hpp>

#include "detail/cache.hpp"
#include "detail/volume.hpp"

namespace zi {
namespace mesh {

struct int_mesh_cache
{
public:
    typedef zi::vl::vec<uint32_t, 4> key_type;


private:
    struct int_mesh_cache_impl
    {
    private:
        boost::shared_ptr<zi::mesh::int_mesh>     value_  ;
        bool                                      opened_ ;
        bool                                      opening_;
        zi::mutex              m_   ;
        zi::condition_variable cv_  ;

    public:
        int_mesh_cache_impl()
            : value_()
            , opened_(0)
            , opening_(0)
            , m_()
            , cv_()
        { }

        int_mesh_cache_impl(shared_ptr<zi::mesh::int_mesh> v)
            : value_(v)
            , opened_(1)
            , opening_(1)
            , m_()
            , cv_()
        { }

        void open( const key_type& c )
        {
            bool should_open = false;
            bool should_wait = false;

            {
                zi::mutex::guard g(m_);
                if ( !opened_ )
                {
                    should_wait = true;
                    if ( !opening_ )
                    {
                        opening_    = true;
                        should_open = true;
                    }
                }
            }

            if ( should_open )
            {
                value_  = zi::mesh::vol::load_int_mesh(c);
                {
                    zi::mutex::guard g(m_);
                    opened_  = true ;
                    cv_.notify_all();
                    return;
                }
            }

            if ( should_wait )
            {
                zi::mutex::guard g(m_);
                while ( !opened_ )
                {
                    cv_.wait(m_);
                }
            }
        }

        std::size_t size( const key_type& c )
        {
            open(c);
            {
                if ( value_ )
                {
                    return value_->mem_size();
                }
                else
                {
                    return 0;
                }
            }
        }

        boost::shared_ptr<zi::mesh::int_mesh> get( const key_type& c )
        {
            open(c);
            return value_;
        }
    };

private:
    zi::vl::vec<uint32_t, 4>               key_  ;
    boost::shared_ptr<int_mesh_cache_impl> value_;

public:
    int_mesh_cache()
        : key_()
        , value_( new int_mesh_cache_impl )
    { }

    int_mesh_cache( const zi::vl::vec<uint32_t, 4>& key )
        : key_(key)
        , value_( new int_mesh_cache_impl )
    { }

    int_mesh_cache( const zi::vl::vec<uint32_t, 3>& c, uint32_t id )
        : key_(c, id)
        , value_( new int_mesh_cache_impl )
    { }

    int_mesh_cache( const zi::vl::vec<uint32_t, 4>& key,
                    boost::shared_ptr<zi::mesh::int_mesh> v)
        : key_(key)
        , value_( new int_mesh_cache_impl(v) )
    { }

    int_mesh_cache( const zi::vl::vec<uint32_t, 3>& c, uint32_t id,
                    boost::shared_ptr<zi::mesh::int_mesh> v)
        : key_(c, id)
        , value_( new int_mesh_cache_impl(v) )
    { }

    const key_type& key() const
    {
        return key_;
    }

    std::size_t size()
    {
        return value_->size(key_);
    }

    void open()
    {
        return value_->open(key_);
    }

    boost::shared_ptr<zi::mesh::int_mesh> get()
    {
        return value_->get(key_);
    }

};

struct face_mesh_cache
{
public:
    typedef zi::vl::vec<uint32_t, 5> key_type;


private:
    struct face_mesh_cache_impl
    {
    private:
        boost::shared_ptr<zi::mesh::face_mesh<double> >    value_  ;
        bool                                               opened_ ;
        bool                                               opening_;
        zi::mutex              m_   ;
        zi::condition_variable cv_  ;

    public:
        face_mesh_cache_impl()
            : value_()
            , opened_(0)
            , opening_(0)
            , m_()
            , cv_()
        { }

        void open( const key_type& c )
        {
            bool should_open = false;
            bool should_wait = false;

            {
                zi::mutex::guard g(m_);
                if ( !opened_ )
                {
                    should_wait = true;
                    if ( !opening_ )
                    {
                        opening_    = true;
                        should_open = true;
                    }
                }
            }

            if ( should_open )
            {
                value_ = zi::mesh::vol::load_face_mesh<double>(c);
                {
                    zi::mutex::guard g(m_);
                    opened_ = true;
                    cv_.notify_all();
                    return;
                }
            }

            if ( should_wait )
            {
                zi::mutex::guard g(m_);
                while ( !opened_ )
                {
                    cv_.wait(m_);
                }
            }
        }

        std::size_t size( const key_type& c )
        {
            open(c);
            {
                if ( value_ )
                {
                    return value_->mem_size();
                }
                else
                {
                    return 0;
                }
            }
        }

        boost::shared_ptr<zi::mesh::face_mesh<double> > get( const key_type& c )
        {
            open(c);
            return value_;
        }
    };

private:
    key_type                                key_  ;
    boost::shared_ptr<face_mesh_cache_impl> value_;

public:
    face_mesh_cache()
        : key_()
        , value_( new face_mesh_cache_impl )
    { }

    face_mesh_cache( const key_type& key )
        : key_(key)
        , value_( new face_mesh_cache_impl )
    { }

    face_mesh_cache( const zi::vl::vec<uint32_t, 4>& c, uint32_t id )
        : key_(c, id)
        , value_( new face_mesh_cache_impl )
    { }

    const key_type& key() const
    {
        return key_;
    }

    std::size_t size()
    {
        return value_->size(key_);
    }

    void open()
    {
        return value_->open(key_);
    }

    boost::shared_ptr<zi::mesh::face_mesh<double> > get()
    {
        return value_->get(key_);
    }

};

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_MESH_CACHE_HPP
