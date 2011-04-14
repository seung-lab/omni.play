#ifndef OM_THREAD_POOL_BY_MIP_LEVEL_HPP
#define OM_THREAD_POOL_BY_MIP_LEVEL_HPP

#include "common/om.hpp"
#include "utility/omSystemInformation.h"
#include "threads/omThreadPoolManager.h"
#include "zi/omThreads.h"
#include "tiles/cache/omMipLevelContainer.hpp"

class OmThreadPoolByMipLevel {
private:
    typedef zi::concurrency_::task_manager_tpl<OmMipLevelContainer> task_man_t;
    boost::scoped_ptr<task_man_t> pool_;

public:
    OmThreadPoolByMipLevel(){
        OmThreadPoolManager::Add(this);
    }

    virtual ~OmThreadPoolByMipLevel()
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

    inline void insert(const int mipLevel, const om::shared_ptr<zi::runnable>& job)
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

#endif
