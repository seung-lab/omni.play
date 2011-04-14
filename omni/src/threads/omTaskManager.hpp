#ifndef OM_TASK_MANAGER_HPP
#define OM_TASK_MANAGER_HPP

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

#include "threads/omTaskManagerImpl.hpp"

#include <zi/bits/type_traits.hpp>
#include <zi/meta/enable_if.hpp>

template< class TaskContainer >
class OmTaskManager {
private:
    typedef zi::shared_ptr< zi::concurrency_::runnable > task_t;

    TaskContainer tasks_;

    // shared_ptr to support enable_shared_from_this
    zi::shared_ptr<OmTaskManagerImpl<TaskContainer> > manager_;

public:
    OmTaskManager( std::size_t worker_limit,
                   std::size_t max_size = std::numeric_limits< std::size_t >::max() )
        : manager_(new OmTaskManagerImpl<TaskContainer>(worker_limit, max_size, tasks_))
    {}

    ~OmTaskManager()
    {}

    std::size_t empty()
    {
        return tasks_.empty();
    }

    std::size_t idle()
    {
        return manager_->empty();
    }

    std::size_t size()
    {
        return tasks_.size();
    }

    std::size_t worker_count()
    {
        return manager_->worker_count();
    }

    std::size_t worker_limit()
    {
        return manager_->worker_limit();
    }

    std::size_t idle_workers()
    {
        return manager_->idle_workers();
    }

    bool start()
    {
        return manager_->start();
    }

    void stop( bool and_join = false )
    {
        return manager_->stop( and_join );
    }

    void join()
    {
        manager_->join();
    }

    void clear()
    {
        tasks_.clear();
    }

    void add_workers( std::size_t count )
    {
        manager_->add_workers( count );
    }

    void remove_workers( std::size_t count )
    {
        manager_->remove_workers( count );
    }

//push_front
    void push_front(task_t task)
    {
        tasks_.push_front(task);
        manager_->wake_all();
    }

    template< class Runnable >
    void push_front( zi::shared_ptr< Runnable > task)
    {
        tasks_.push_front(task);
        manager_->wake_all();
    }

    template< class Function >
    void push_front(const Function& task)
    {
        tasks_.push_front(task);
        manager_->wake_all();
    }

//push_back
    void push_back(task_t task)
    {
        tasks_.push_back(task);
        manager_->wake_all();
    }

    template< class Runnable >
    void push_back( zi::shared_ptr< Runnable > task)
    {
        tasks_.push_back(task);
        manager_->wake_all();
    }

    template< class Function >
    void push_back(const Function& task)
    {
        tasks_.push_back(task);
        manager_->wake_all();
    }

//insert
    void insert(task_t task)
    {
        tasks_.insert(task);
        manager_->wake_all();
    }

    template< class Runnable >
    void insert( zi::shared_ptr< Runnable > task)
    {
        tasks_.insert(task);
        manager_->wake_all();
    }

    template< class Function >
    void insert(const Function& task)
    {
        tasks_.insert(task);
        manager_->wake_all();
    }

//insert w/ arg
    template <typename ARG>
    void insert(const ARG& arg, task_t task)
    {
        tasks_.insert(arg, task);
        manager_->wake_all();
    }

    template <typename ARG, class Runnable>
    void insert(const ARG& arg, zi::shared_ptr< Runnable > task)
    {
        tasks_.insert(arg, task);
        manager_->wake_all();
    }

    template <typename ARG, class Function>
    void insert(const ARG& arg, const Function& task)
    {
        tasks_.insert(arg, task);
        manager_->wake_all();
    }
};

#endif

