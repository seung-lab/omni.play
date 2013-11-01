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

#ifndef ZI_MESHING_DETAIL_GARBAGE_HPP
#define ZI_MESHING_DETAIL_GARBAGE_HPP

#include <zi/utility/singleton.hpp>
#include <zi/concurrency.hpp>

#include <list>

namespace zi {
namespace mesh {

struct thrash_base
{
    virtual ~thrash_base() {};
};

template < typename T >
struct thrash: thrash_base
{
    const T t_;

    thrash( const T& t )
        : t_(t)
    { }
};


class garbage_impl
{
private:
    std::list<thrash_base*> g_;
    zi::mutex               m_;
    zi::condition_variable  cv_;
    bool                    w_;
    bool                    e_;
    zi::thread              t_;

    void loop()
    {
        while (1)
        {
            {
                zi::mutex::guard g(m_);
                while ( !w_ )
                {
                    cv_.wait(m_);
                }
            }

            // w_ is true now

            while ( 1 )
            {
                std::list<thrash_base*> to_thrash;
                {
                    zi::mutex::guard g(m_);
                    std::swap(to_thrash, g_);

                    if ( to_thrash.size() == 0 )
                    {
                        w_ = false;

                        if ( e_ )
                        {
                            return;
                        }

                        break;
                    }
                }

                while ( to_thrash.size() )
                {
                    delete to_thrash.front();
                    to_thrash.pop_front();
                }
            }
        }
    }

public:
    garbage_impl()
        : g_()
        , m_()
        , cv_()
        , w_(0)
        , t_(zi::run_fn(zi::bind(&garbage_impl::loop, this)))
    {
        t_.start();
    }

    ~garbage_impl()
    {
        {
            zi::mutex::guard g(m_);
            e_ = true;

            if ( !w_ )
            {
                w_ = true;
                cv_.notify_one();
            }
        }

        t_.join();
    }

    template< typename T >
    void add( const T& t )
    {
        thrash_base* x = new thrash<T>(t);
        {
            zi::mutex::guard g(m_);

            if ( e_ )
            {
                return;
            }

            g_.push_back(x);

            if ( !w_ )
            {
                w_ = true;
                cv_.notify_one();
            }
        }
    }

}; // class garbage

namespace {

garbage_impl& garbage = zi::singleton<garbage_impl>::instance();

} // namespace


} // namespace mesh
} // namespace zi


#endif // ZI_MESHING_DETAIL_GARBAGE_HPP
