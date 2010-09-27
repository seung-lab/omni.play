#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include "zi/omThreads.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class OmThreadPool {
private:
	typedef zi::task_manager::simple taskman_t;
	boost::shared_ptr<taskman_t> taskMan_;

public:
	OmThreadPool() {}

	~OmThreadPool(){
		stop();
	}

	void start(const int numWorkerThreads){
		taskMan_ = boost::make_shared<taskman_t>(numWorkerThreads);
		taskMan_->start();
	}

	void join(){
		taskMan_->join();
	}

	void clear(){
		if(getTaskCount()){
			taskMan_->clear();
		}
	}

	void stop(){
		clear();
		taskMan_->stop();
	}

	void addTaskFront(const boost::shared_ptr<zi::runnable>& job){
		taskMan_->push_front(job);
	}

	void addTaskBack(const boost::shared_ptr<zi::runnable>& job){
		taskMan_->push_back(job);
	}

	int getTaskCount() const {
		return taskMan_->size();
	}

	int getNumWorkerThreads() const {
		return taskMan_->worker_count();
	}

	int getMaxSimultaneousTaskCount() const {
		return getNumWorkerThreads();
	}
};

#endif
