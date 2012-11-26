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

#ifndef ZI_MESHING_DETAIL_SCRUM_HPP
#define ZI_MESHING_DETAIL_SCRUM_HPP

#include <zi/concurrency.hpp>
#include <list>

namespace zi {
namespace mesh {

class scrum: non_copyable
{
private:
    zi::mutex                m_         ;
    zi::mutex                master_m_  ;
    zi::condition_variable   master_cv_ ;
    zi::condition_variable   slave_cv_  ;
    mutable std::list<bool*> list_      ;
    mutable std::size_t      n_         ;

public:
    scrum()
        : m_()
        , master_m_()
        , master_cv_()
        , slave_cv_()
        , list_(0)
        , n_(0)
    { }

    ~scrum()
    { }

    void summon( std::size_t n ) const
    {
        master_m_.lock();
        zi::mutex::guard g(m_);

        n_ = n;

        while ( list_.size() < n )
        {
            master_cv_.wait(m_);
        }
    }

    void dismiss() const
    {
        zi::mutex::guard g(m_);

        ZI_ASSERT(n_>0);
        ZI_ASSERT(list_.size()>=n_);

        for ( std::size_t i = 0; i < n_; ++i )
        {
            *(list_.front()) = true;
            list_.pop_front();
        }

        slave_cv_.notify_all();

        while ( n_ != 0 )
        {
            master_cv_.wait(m_);
        }

        master_m_.unlock();
    }

    void arrive() const
    {
        bool me = false;

        zi::mutex::guard g(m_);

        list_.push_back(&me);

        if ( list_.size() >= n_ )
        {
            master_cv_.notify_one();
        }

        while ( !me )
        {
            slave_cv_.wait(m_);
        }

        --n_;

        if ( n_ == 0 )
        {
            master_cv_.notify_one();
        }
    }

}; // class scrum

} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_SCRUM_HPP

