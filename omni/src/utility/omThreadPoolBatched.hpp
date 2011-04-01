#ifndef OM_THREAD_POOL_BATCHED_HPP
#define OM_THREAD_POOL_BATCHED_HPP

#include "utility/omThreadPool.hpp"

template <class ARG, class T>
class OmThreadPoolBatched {
private:
    boost::function<void (T*, const ARG& arg)> func_;
    T* classInstantiation_;

    OmThreadPool pool_;

    static const size_t TaskVecSize = 1000;
    std::vector<ARG> tasks_;

public:
    OmThreadPoolBatched()
    {
        pool_.start(1);
        tasks_.reserve(TaskVecSize);
    }

    ~OmThreadPoolBatched()
    {
        JoinPool();
    }

    void SetFunc(boost::function<void (T*, const ARG& arg)> func,
                 T* classInstantiation)
    {
        func_ = func;
        classInstantiation_ = classInstantiation;
    }

    inline void AddOrSpawnTasks(const ARG& t)
    {
        if(tasks_.size() >= TaskVecSize)
        {
            pool_.push_back(
                zi::run_fn(
                    zi::bind(&OmThreadPoolBatched::worker,
                             this, tasks_)));

            tasks_.clear();
            tasks_.reserve(TaskVecSize);
        }

        tasks_.push_back(t);
    }

    void JoinPool()
    {
        if(!tasks_.empty())
        {
            pool_.push_back(
                zi::run_fn(
                    zi::bind(&OmThreadPoolBatched::worker,
                             this, tasks_)));
            tasks_.clear();
        }

        pool_.join();
    }

private:

    // work on copy of tasks vec
    void worker(std::vector<ARG> tasks)
    {
        const size_t size = tasks.size();

        for(size_t i = 0; i < size; ++i)
        {
            func_(classInstantiation_, tasks[i]);
        }
    }
};

#endif
