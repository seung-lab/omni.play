#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include "common/om.hpp"
#include "utility/omSystemInformation.h"
#include "utility/omThreadPoolManager.h"
#include "zi/omThreads.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class OmThreadPool {
private:
	boost::shared_ptr<zi::task_manager::simple> pool_;

public:
	OmThreadPool(){
		OmThreadPoolManager::Add(this);
	}

	virtual ~OmThreadPool()
	{
		pool_->stop();
		OmThreadPoolManager::Remove(this);
	}

	void start()
	{
		const int numWokers = OmSystemInformation::get_num_cores();
		start(numWokers);
	}

	void start(const uint32_t numWorkerThreads)
	{
		if(!numWorkerThreads){
			throw OmIoException("invalid num of threads");
		}

		pool_ = boost::make_shared<zi::task_manager::simple>(numWorkerThreads);
		pool_->start();
	}

	void join(){
		pool_->join();
	}

	void clear()
	{
		if(pool_->size()){
			pool_->clear();
		}
	}

	void stop()
	{
		clear();
		pool_->stop();
	}

	void addTaskFront(const boost::shared_ptr<zi::runnable>& job){
		pool_->push_front(job);
	}

	void addTaskBack(const boost::shared_ptr<zi::runnable>& job){
		pool_->push_back(job);
	}

    template <typename T>
    void push_front(const T& task){
        pool_->push_front(task);
    }

    template <typename T>
    void push_back(const T& task){
        pool_->push_back(task);
    }

	int getTaskCount() const {
		return pool_->size();
	}

	int getNumWorkerThreads() const {
		return pool_->worker_count();
	}

	int getMaxSimultaneousTaskCount() const {
		return getNumWorkerThreads();
	}
};

#endif
