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
        const int numWokers = OmSystemInformation::get_num_cores();
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

    inline void addTaskFront(const boost::shared_ptr<zi::runnable>& job){
        pool_->push_front(job);
    }

    inline void addTaskBack(const boost::shared_ptr<zi::runnable>& job){
        pool_->push_back(job);
    }

    template <typename T>
    inline void push_front(const T& task){
        pool_->push_front(task);
    }

    template <typename T>
    inline void push_back(const T& task){
        pool_->push_back(task);
    }

    inline int getTaskCount() const {
        return pool_->size();
    }

    inline int getNumWorkerThreads() const {
        return pool_->worker_count();
    }

    inline int getMaxSimultaneousTaskCount() const {
        return getNumWorkerThreads();
    }
};

#endif
