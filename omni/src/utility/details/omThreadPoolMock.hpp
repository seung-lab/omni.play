#ifndef OM_THREAD_POOL_MOCK_HPP
#define OM_THREAD_POOL_MOCK_HPP

#include "common/omDebug.h"
#include "utility/details/omIThreadPool.h"

class OmThreadPoolMock : public OmIThreadPool {
public:
	virtual ~OmThreadPoolMock(){}

	void DoStart(const int){
		debug(threadpool, "fake threadpool created (no threads)\n");
	}
	void DoJoin(){}
	void DoClear(){}
	void DoStop(){}

	void DoAddTaskFront(const boost::shared_ptr<zi::runnable>&){
		assert(0 && "mock omthreadpool");
	}

	void DoAddTaskBack(const boost::shared_ptr<zi::runnable>&){
		assert(0 && "mock omthreadpool");
	}

	int DoGetTaskCount() const { return 0;}
	int DoGetNumWorkerThreads() const { return 0; }
	int DoGetMaxSimultaneousTaskCount() const { return 0; }
};

#endif
