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

#ifndef ZI_MESHING_DETAIL_LOG_HPP
#define ZI_MESHING_DETAIL_LOG_HPP

#include <zi/utility/for_each.hpp>
#include <zi/utility/singleton.hpp>
#include <zi/utility/non_copyable.hpp>
#include <zi/concurrency.hpp>
#include <zi/time.hpp>

#include <iostream>
#include <sstream>
#include <list>

namespace zi {
namespace mesh {

class log_output_impl
{
private:
    zi::mutex                      m_;
    std::list<std::ostringstream*> o_;
    bool                           d_;
    zi::thread                     t_;

    void loop()
    {
        bool done = false;
        while (!done)
        {
            std::list<std::ostringstream*> l;
            {
                zi::mutex::guard g(m_);
                l.swap(o_);
                done = d_;
            }

            FOR_EACH( it, l )
            {
                std::cout << (*it)->str() << "\n";
                delete (*it);
            }

            std::cout << std::flush;

            zi::this_thread::usleep(200000);
        }
    }

public:
    log_output_impl()
        : m_()
        , o_()
        , d_(false)
        , t_(zi::run_fn(zi::bind(&log_output_impl::loop,this)))
    {
    }

    ~log_output_impl()
    {
        {
            zi::mutex::guard g(m_);
            d_ = true;
        }
        if (t_.get_id()) {
            t_.join();
        }
    }

    void start()
    {
        t_.start();
    }

    void reg(std::ostringstream* o)
    {
        zi::mutex::guard g(m_);
        o_.push_back(o);
    }
}; // class log_output_impl

namespace {

log_output_impl& log_output =
    zi::singleton<log_output_impl>::instance();

} // namespace

class log_token: non_copyable
{
public:
    std::ostringstream* i_;

    log_token()
        : i_(new std::ostringstream)
    { }

    ~log_token()
    {
        log_output.reg(i_);
    }

    template< typename T >
    log_token& operator<< ( const T& v )
    {
        (*i_) << v;
        return *this;
    }
}; // log_token


#define LOG(what) (log_token()) << "LOG(" << #what << ") "      \
    << "[" << zi::now::usecs() << "] "



} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_LOG_HPP
