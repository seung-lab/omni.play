#ifndef OM_I_THREADPOOL_H
#define OM_I_THREADPOOL_H

#include "zi/omThreads.h"

class OmIThreadPool {
public:
	virtual void DoStart(const int) = 0;
	virtual void DoJoin() = 0;
	virtual void DoClear() = 0;
	virtual void DoStop() = 0;

	virtual void DoAddTaskFront(const boost::shared_ptr<zi::runnable>&) = 0;
	virtual void DoAddTaskBack(const boost::shared_ptr<zi::runnable>&) = 0;

	virtual int DoGetTaskCount() const = 0;
	virtual int DoGetNumWorkerThreads() const = 0;
	virtual int DoGetMaxSimultaneousTaskCount() const = 0;
};

#endif
