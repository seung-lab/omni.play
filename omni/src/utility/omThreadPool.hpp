#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include "common/om.hpp"
#include "utility/details/omIThreadPool.h"
#include "utility/details/omThreadPoolMock.hpp"
#include "utility/details/omThreadPoolImpl.hpp"
#include "utility/omSystemInformation.h"

#include "utility/omThreadPoolManager.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class OmThreadPool {
private:
	boost::shared_ptr<OmIThreadPool> impl_;

public:
	OmThreadPool(){
		OmThreadPoolManager::Add(this);
	}

	virtual ~OmThreadPool(){
		stop();
		OmThreadPoolManager::Remove(this);
	}

	void start(){
		const int numWokers = OmSystemInformation::get_num_cores();
		start(numWokers);
	}

	void start(const int numWorkerThreads){
		if(numWorkerThreads){
			impl_ = boost::make_shared<OmThreadPoolImpl>();
		} else {
			impl_ = boost::make_shared<OmThreadPoolMock>();
		}
		impl_->DoStart(numWorkerThreads);
	}

	void join(){
		impl_->DoJoin();
	}

	void clear(){
		impl_->DoClear();
	}

	void stop(){
		impl_->DoStop();
	}

	void addTaskFront(const boost::shared_ptr<zi::runnable>& job){
		impl_->DoAddTaskFront(job);
	}

	void addTaskBack(const boost::shared_ptr<zi::runnable>& job){
		impl_->DoAddTaskBack(job);
	}

	int getTaskCount() const {
		return impl_->DoGetTaskCount();
	}

	int getNumWorkerThreads() const {
		return impl_->DoGetNumWorkerThreads();
	}

	int getMaxSimultaneousTaskCount() const {
		return impl_->DoGetMaxSimultaneousTaskCount();
	}
};

#endif
