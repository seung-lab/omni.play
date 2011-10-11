#pragma once

//
// Copyright (C) 2010  Aleksandar Zlateski <zlateski@mit.edu>
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

#include "zi/omMutex.h"
#include "utility/omSharedPtr.hpp"

#include <zi/concurrency/runnable.hpp>
#include <deque>

class taskManagerContainerDeque {
private:
    typedef zi::shared_ptr< zi::concurrency_::runnable > task_t;
    std::deque<task_t> queue_;

    zi::spinlock lock_;

    template< class Function >
    inline task_t wrapFunction(const Function& task)
    {
        return om::shared_ptr< zi::concurrency_::runnable_function_wrapper >
                           ( new zi::concurrency_::runnable_function_wrapper( task ) );
    }

public:
    std::size_t size() const
    {
        zi::guard g(lock_);
        return queue_.size();
    }

    bool empty() const
    {
        zi::guard g(lock_);
        return queue_.empty();
    }

    void clear()
    {
        zi::guard g(lock_);
        queue_.clear();
    }

    task_t get_front()
    {
        zi::guard g(lock_);
        if(queue_.empty()){
            return task_t();
        }
        task_t ret = queue_.front();
        queue_.pop_front();
        return ret;
    }

//push_front
    void push_front(task_t task)
    {
        zi::guard g(lock_);
        queue_.push_front( task );
    }

    template< class Runnable >
    void push_front(om::shared_ptr<Runnable> task)
    {
        zi::guard g(lock_);
        queue_.push_front( task );
    }

    template< class Function >
    void push_front(const Function& task)
    {
        zi::guard g(lock_);
        queue_.push_front(wrapFunction(task));
    }

//push_back
    void push_back(task_t task)
    {
        zi::guard g(lock_);
        queue_.push_back( task );
    }

    template< class Runnable >
    void push_back(om::shared_ptr<Runnable> task)
    {
        zi::guard g(lock_);
        queue_.push_back( task );
    }

    template< class Function >
    void push_back(const Function& task)
    {
        zi::guard g(lock_);
        queue_.push_back(wrapFunction(task));
    }
};

