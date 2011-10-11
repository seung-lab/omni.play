#pragma once

#include "utility/systemInformation.h"
#include "threads/taskManagerManager.h"
#include "zi/threads.h"
#include "tiles/cache/mipLevelContainer.hpp"

namespace om {
namespace threads {

class threadPoolByMipLevel {
private:
    typedef zi::concurrency_::task_manager_tpl<OmMipLevelContainer> task_man_t;
    boost::scoped_ptr<task_man_t> pool_;

public:
    threadPoolByMipLevel(){
        threadPoolManager::Add(this);
    }

    virtual ~threadPoolByMipLevel()
    {
        // remove before stopping (else threadPoolManager may also attempt to
        //   stop pool during its own shutdown...)
        threadPoolManager::Remove(this);
        if(pool_){
            pool_->stop();
        }
    }

    bool wasStarted(){
        return pool_;
    }

    void start()
    {
        int numWokers = systemInformation::get_num_cores();
        if(numWokers < 2){
            numWokers = 2;
        }
        start(numWokers);
    }

    void start(const uint32_t numWorkerThreads)
    {
        if(!numWorkerThreads){
            throw common::ioException("please specify more than 0 threads");
        }

        pool_.reset(new task_man_t(numWorkerThreads));
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

    inline void insert(const int mipLevel, const boost::shared_ptr<zi::runnable>& job)
    {
        assert(pool_ && "pool not started");
        pool_->insert(mipLevel, job);
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

} // namespace threads
} // namespace om
