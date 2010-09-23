#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <zi/threads>

class OmThreadPool {
public:
	OmThreadPool() {}

	~OmThreadPool(){
		stop();
	}

	void start(const int numWorkerThreads){
		boost::shared_ptr<zi::ThreadFactory> factory(new zi::ThreadFactory);
		factory->setDetached(false);

		threadManager_ = boost::make_shared<zi::ThreadManager>
			(factory, numWorkerThreads);

		threadManager_->start();
	}

	void join(){
		threadManager_->join();
	}

	void clear(){
		if(getTaskCount()){
			threadManager_->clearTasks();
		}
	}

	void stop(){
		clear();
		threadManager_->stop();
	}

	void addTaskFront(const boost::shared_ptr<zi::Runnable>& job){
		threadManager_->pushTask(job);
	}

	void addTaskBack(const boost::shared_ptr<zi::Runnable>& job){
		threadManager_->addTask(job);
	}

	int getTaskCount() const {
		return threadManager_->getTaskCount();
	}

	int getNumWorkerThreads() const {
		return threadManager_->getWorkerCount();
	}

	int getMaxSimultaneousTaskCount() const {
		return getNumWorkerThreads();
	}

private:
	boost::shared_ptr<zi::ThreadManager> threadManager_;
};

#endif
