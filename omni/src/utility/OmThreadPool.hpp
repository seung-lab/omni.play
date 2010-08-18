#ifndef OM_THREAD_POOL_HPP
#define OM_THREAD_POOL_HPP

#include <boost/shared_ptr.hpp>
#include <zi/threads>

class OmThreadPool {
public:
	OmThreadPool(){}

	~OmThreadPool(){
		stop();
	}

	void start(const int numWorkerThreads){
		boost::shared_ptr<zi::ThreadFactory> factory(new zi::ThreadFactory);
		factory->setDetached(false);

		threadManager_ = boost::shared_ptr<zi::ThreadManager>
			(new zi::ThreadManager(factory, numWorkerThreads));

		threadManager_->start();
	}

	void stop(){
		if(getTaskCount()){
			threadManager_->clearTasks();
		}
		threadManager_->stop();
	}

	void addTaskFront(boost::shared_ptr<zi::Runnable> job){
		threadManager_->pushTask(job);
	}

	void addTaskBack(boost::shared_ptr<zi::Runnable> job){
		threadManager_->addTask(job);
	}

	int getTaskCount() const {
		return threadManager_->getTaskCount();
	}

private:
	boost::shared_ptr<zi::ThreadManager> threadManager_;
};

#endif
