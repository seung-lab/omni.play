#pragma once

#include "zi/omMutex.h"
#include "utility/omSharedPtr.hpp"

#include <zi/concurrency/runnable.hpp>
#include <map>

class OmTaskManagerContainerMipSorted {
private:
    typedef zi::shared_ptr< zi::concurrency_::runnable > task_t;

    // sort by mip level, highest (most downsampled) first
    std::multimap<int, task_t, std::greater<int> > queue_;

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
        task_t ret = queue_.begin()->second;
        queue_.erase(queue_.begin());
        return ret;
    }

//insert w/ arg
    template <typename ARG>
    void insert(const ARG& arg, task_t task)
    {
        zi::guard g(lock_);
        queue_.insert(std::make_pair(arg, task));
    }

    template <typename ARG, class Runnable >
    void insert(const ARG& arg, om::shared_ptr<Runnable> task)
    {
        zi::guard g(lock_);
        queue_.insert(std::make_pair(arg, task));
    }

    template <typename ARG, class Function >
    void insert(const ARG& arg, const Function& task)
    {
        zi::guard g(lock_);
        queue_.insert(std::make_pair(arg, task));
    }
};

