#ifndef OM_THREAD_POOL_IMPL_HPP
#define OM_THREAD_POOL_IMPL_HPP

#include "utility/details/omIThreadPool.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class OmThreadPoolImpl : public OmIThreadPool {
private:
	typedef zi::task_manager::simple taskman_t;
	boost::shared_ptr<taskman_t> taskMan_;

public:
	virtual ~OmThreadPoolImpl(){
		DoStop();
	}

	void DoStart(const int numWorkerThreads){
		taskMan_ = boost::make_shared<taskman_t>(numWorkerThreads);
		taskMan_->start();
		debug(threadpool, "threadpool created\n");
	}

	void DoJoin(){
		taskMan_->join();
	}

	void DoClear(){
		if(DoGetTaskCount()){
			taskMan_->clear();
		}
	}

	void DoStop(){
		DoClear();
		taskMan_->stop();
	}

	void DoAddTaskFront(const boost::shared_ptr<zi::runnable>& job){
		taskMan_->push_front(job);
	}

	void DoAddTaskBack(const boost::shared_ptr<zi::runnable>& job){
		taskMan_->push_back(job);
	}

	int DoGetTaskCount() const {
		return taskMan_->size();
	}

	int DoGetNumWorkerThreads() const {
		return taskMan_->worker_count();
	}

	int DoGetMaxSimultaneousTaskCount() const {
		return DoGetNumWorkerThreads();
	}
};

#endif
