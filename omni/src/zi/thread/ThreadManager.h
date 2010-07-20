/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_THREADS_THREADMANAGER_H_
#define _ZI_THREADS_THREADMANAGER_H_

#include <boost/shared_ptr.hpp>
#include <sys/types.h>
#include <set>
#include <queue>

#include "ThreadFactory.h"
#include "Monitor.h"

namespace zi { namespace Threads {

class ThreadManager {
private:
  class Task;
  class Worker;
public:

  ThreadManager(boost::shared_ptr<ThreadFactory> threadFactory,
                int workerCount,
                int maxQueueSize = 0);

  enum STATE {
    WAITING,
    RUNNING,
    STOPPING,
    DONE
  };

  ~ThreadManager();

  void start();
  void stop(bool andJoin = false);
  void join();

  int  getWorkerCount() const;
  //void setWorkerCount(int n);

  int  getActiveWorkerCount() const;
  int  getTaskCount()         const;

  void addTask(boost::shared_ptr<Runnable> job, int64_t ttl = 0);

private:
  void workerLoop(Worker *worker);
  void addWorkers(int nWorkers);
  void removeWorkers(int nWorkers);

  boost::shared_ptr<ThreadFactory> threadFactory_;

  int activeWorkers_ ;
  int maxWorkers_    ;
  int maxQueueSize_  ;
  int idleWorkers_   ;

  STATE state_;

  Monitor managerMonitor_;
  Monitor workerMonitor_;

  std::queue<boost::shared_ptr<Task> > tasks_;
  std::set<boost::shared_ptr<Thread> > workers_;
  std::set<boost::shared_ptr<Thread> > garbage_;

  friend class Task;
  friend class Worker;
};

} }

#endif
