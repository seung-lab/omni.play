#ifndef OM_TASK_MANAGER_CONTAINER_HPP
#define OM_TASK_MANAGER_CONTAINER_HPP

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

#include <zi/concurrency/runnable.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <deque>

class OmTaskManagerContainerDeque {
private:
    typedef zi::shared_ptr< zi::concurrency_::runnable > task_t;
    std::deque<task_t> queue_;

    zi::spinlock lock_;

    template< class Function >
    inline task_t wrapFunction(const Function& task)
    {
        return zi::shared_ptr< zi::concurrency_::runnable_function_wrapper >
                           ( new zi::concurrency_::runnable_function_wrapper( task ) );
    }

public:
    std::size_t size()
    {
        zi::guard g(lock_);
        return queue_.size();
    }

    bool empty()
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
    void push_front(zi::shared_ptr<Runnable> task)
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
    void push_back(zi::shared_ptr<Runnable> task)
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

//insert
    void insert(task_t task)
    {
        // locked in call
        push_front(task);
    }

    template< class Runnable >
    void insert(zi::shared_ptr<Runnable> task)
    {
        // locked in call
        push_front(task);
    }

    template< class Function >
    void insert(const Function& task)
    {
        // locked in call
        push_front(task);
    }

//insert w/ arg
    template <typename ARG>
    void insert(const ARG&, task_t task)
    {
        // locked in call
        push_front(task);
    }

    template <typename ARG, class Runnable >
    void insert(const ARG&, zi::shared_ptr<Runnable> task)
    {
        // locked in call
        push_front(task);
    }

    template <typename ARG, class Function >
    void insert(const ARG&, const Function& task)
    {
        // locked in call
        push_front(task);
    }
};

#endif
