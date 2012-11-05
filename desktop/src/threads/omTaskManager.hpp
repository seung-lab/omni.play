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

#include "threads/omTaskManagerImpl.hpp"
#include "threads/omTaskManagerTypes.h"
#include "utility/omSystemInformation.h"

#include <zi/bits/type_traits.hpp>
#include <zi/meta/enable_if.hpp>

template <typename TaskContainer>
class OmTaskManager {
private:
    typedef boost::shared_ptr<zi::concurrency_::runnable> task_t;

    TaskContainer tasks_;

    // shared_ptr to support enable_shared_from_this
    typedef OmTaskManagerImpl<TaskContainer> manager_t;
    boost::shared_ptr<manager_t> manager_;

public:
    OmTaskManager()
    {}

    ~OmTaskManager(){
        join();
    }

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

    void start()
    {
        int numWokers = OmSystemInformation::get_num_cores();
        if(numWokers <2){
            numWokers = 2;
        }
        start(numWokers);
    }

    void start(const uint32_t numWorkerThreads)
    {
        if(!numWorkerThreads){
            throw IoException("please specify more than 0 threads");
        }

        const uint32_t max_size = std::numeric_limits<uint32_t>::max();

        manager_.reset(new manager_t(numWorkerThreads, max_size, tasks_));
        manager_->start();
    }

    void stop()
    {
        if(!manager_){
            return;
        }
        clear();
        manager_->stop();
        manager_.reset();
    }

    void join()
    {
        if(!manager_){
            return;
        }
        manager_->join();
        manager_.reset();
    }

    void clear()
    {
        tasks_.clear();
    }

    void add_workers(std::size_t count)
    {
        manager_->add_workers(count);
    }

    void remove_workers(std::size_t count)
    {
        manager_->remove_workers(count);
    }

//status
    bool wasStarted()
    {
        return manager_;
    }

    inline int getTaskCount() const
    {
        return tasks_.size();
    }

    inline int getNumWorkerThreads() const
    {
        assert(manager_ && "pool not started");
        return manager_->worker_count();
    }

    inline int getMaxSimultaneousTaskCount() const
    {
        assert(manager_ && "pool not started");
        return getNumWorkerThreads();
    }

//push_front
    void push_front(task_t task)
    {
        tasks_.push_front(task);
        wake_manager();
    }

    template <typename Runnable>
    void push_front(boost::shared_ptr<Runnable> task)
    {
        tasks_.push_front(task);
        wake_manager();
    }

    template <typename Function>
    void push_front(const Function& task)
    {
        tasks_.push_front(task);
        wake_manager();
    }

//push_back
    void push_back(task_t task)
    {
        tasks_.push_back(task);
        wake_manager();
    }

    template <typename Runnable>
    void push_back(boost::shared_ptr<Runnable> task)
    {
        tasks_.push_back(task);
        wake_manager();
    }

    template <typename Function>
    void push_back(const Function& task)
    {
        tasks_.push_back(task);
        wake_manager();
    }

//insert
    void insert(task_t task)
    {
        tasks_.insert(task);
        wake_manager();
    }

    template <typename Runnable>
    void insert(boost::shared_ptr<Runnable> task)
    {
        tasks_.insert(task);
        wake_manager();
    }

    template <typename Function>
    void insert(const Function& task)
    {
        tasks_.insert(task);
        wake_manager();
    }

//insert w/ arg
    template <typename ARG>
    void insert(const ARG& arg, task_t task)
    {
        tasks_.insert(arg, task);
        wake_manager();
    }

    template <typename ARG, class Runnable>
    void insert(const ARG& arg, boost::shared_ptr<Runnable> task)
    {
        tasks_.insert(arg, task);
        wake_manager();
    }

    template <typename ARG, class Function>
    void insert(const ARG& arg, const Function& task)
    {
        tasks_.insert(arg, task);
        wake_manager();
    }

private:
    void wake_manager()
    {
        assert(manager_ && "pool not started");
        manager_->wake_all();
    }
};

