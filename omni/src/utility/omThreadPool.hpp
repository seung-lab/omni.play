#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include "common/om.hpp"
#include "utility/omSystemInformation.h"
#include "utility/omThreadPoolManager.h"
#include "zi/omThreads.h"

class OmThreadPool {
private:
    boost::shared_ptr<zi::task_manager::simple> pool_;

public:
    OmThreadPool(){
        OmThreadPoolManager::Add(this);
    }

    virtual ~OmThreadPool()
    {
        // remove before stopping (else OmThreadPoolManager may also attempt to
        //   stop pool during its own shutdown...)
        OmThreadPoolManager::Remove(this);
        if(pool_){
            pool_->stop();
        }
    }

    bool wasStarted(){
        return pool_;
    }

    void start()
    {
        int numWokers = OmSystemInformation::get_num_cores();
        if(numWokers < 2){
            numWokers = 2;
        }
        start(numWokers);
    }

    void start(const uint32_t numWorkerThreads)
    {
        if(!numWorkerThreads){
            throw OmIoException("please specify more than 0 threads");
        }

        pool_.reset(new zi::task_manager::simple(numWorkerThreads));
        pool_->start();
    }

    void join()
    {
        if(!pool_){
            return;
        }
        pool_->join();
        pool_.reset();
    }

    void clear()
    {
        if(!pool_){
            return;
        }
        if(pool_->size()){
            pool_->clear();
        }
    }

    void stop()
    {
        if(!pool_){
            return;
        }
        clear();
        pool_->stop();
        pool_.reset();
    }

    inline void addTaskFront(const boost::shared_ptr<zi::runnable>& job)
    {
        assert(pool_ && "pool not started");
        pool_->push_front(job);
    }

    inline void addTaskBack(const boost::shared_ptr<zi::runnable>& job)
    {
        assert(pool_ && "pool not started");
        pool_->push_back(job);
    }

    template <typename T>
    inline void push_front(const T& task)
    {
        assert(pool_ && "pool not started");
        pool_->push_front(task);
    }

    template <typename T>
    inline void push_back(const T& task)
    {
        assert(pool_ && "pool not started");
        pool_->push_back(task);
    }

    inline int getTaskCount() const
    {
        assert(pool_ && "pool not started");
        return pool_->size();
    }

    inline int getNumWorkerThreads() const
    {
        assert(pool_ && "pool not started");
        return pool_->worker_count();
    }

    inline int getMaxSimultaneousTaskCount() const
    {
        assert(pool_ && "pool not started");
        return getNumWorkerThreads();
    }
};

#endif
