/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <boost/shared_ptr.hpp>

#include "ThreadManager.h"
#include "Exception.h"
#include "zi/base/base.h"

namespace zi { namespace Threads {

using boost::shared_ptr;

class ThreadManager::Task : public Runnable {

public:

  enum TASK_STATE {
    INQUEUE,
    RUNNING,
    DONE
  };

  Task(shared_ptr<Runnable> job) :
    job_(job),
    state_(INQUEUE) {}

  ~Task() {}

  void run() {
    if (state_ == RUNNING) {
      job_->run();
      state_ = DONE;
    }
  }

private:
  shared_ptr<Runnable> job_;
  TASK_STATE state_;
  friend class ThreadManager;
};

class ThreadManager::Worker: public Runnable {

public:

  Worker(ThreadManager* manager) :
    manager_(manager) {}

  ~Worker() {}

  void run() {
    manager_->workerLoop(this);
    return;
  }

private:
  ThreadManager* manager_;
};

void ThreadManager::workerLoop(Worker *worker)
{

  bool active = false;
  bool full   = false;

  {
    Synchronized s(managerMonitor_);
    if (activeWorkers_ < maxWorkers_) {
      active = true;
      activeWorkers_++;
      full = activeWorkers_ == maxWorkers_;
    }
  }

  // notify the manager that we are full
  if (full) {
    Synchronized s(workerMonitor_);
    workerMonitor_.notify();
    full = false;
  }

  while (active) {

    shared_ptr<ThreadManager::Task> task;

    {
      Synchronized s(managerMonitor_);

      while (activeWorkers_ <= maxWorkers_ && tasks_.empty()) {
        idleWorkers_++;
        managerMonitor_.wait();
        idleWorkers_--;
      }

      active = (activeWorkers_ <= maxWorkers_ ||
                ((state_ == STOPPING) && !tasks_.empty()));

      if (active) {

        if (!tasks_.empty()) {
          task = tasks_.front(); tasks_.pop();
          if (task->state_ == ThreadManager::Task::INQUEUE) {
            task->state_    = ThreadManager::Task::RUNNING;
          }
        }

      } else {
        activeWorkers_--;
        full = (activeWorkers_ == maxWorkers_);
      }
    }

    if (task != NULL) {
      if (task->state_ == ThreadManager::Task::RUNNING) {
        try {
          task->run();
        } catch(...) {
          assert(0);
        }
      }
    }
  }

  {
    Synchronized s(workerMonitor_);
    garbage_.insert(worker->thread());

    if (full) {
      workerMonitor_.notify();
    }
  }

}

void ThreadManager::addWorkers(int nWorkers)
{

  if (nWorkers <= 0)
    return;

  {
    Synchronized sm(managerMonitor_);
    maxWorkers_+= nWorkers;

    for (int i=0; i<nWorkers; i++) {
      shared_ptr<Worker> newWorker(new Worker(this));
      shared_ptr<Thread> newThread = threadFactory_->poopThread(newWorker);
      workers_.insert(newThread);
      newThread->start();
    }
  }

  {
    Synchronized s(workerMonitor_);
    while (activeWorkers_ != maxWorkers_)
      workerMonitor_.wait();
  }
}

void ThreadManager::start()
{

  {
    Synchronized s(managerMonitor_);

    if (state_ != ThreadManager::WAITING) {
      throw ThreadException("ThreadManager: already started");
    }

    state_ = ThreadManager::RUNNING;
    managerMonitor_.notifyAll();
  }

}

void ThreadManager::removeWorkers(int nWorkers)
{

  {
    Synchronized s(managerMonitor_);
    nWorkers = (nWorkers > maxWorkers_) ? maxWorkers_ : nWorkers;
    if (nWorkers <= 0)
      return;
    maxWorkers_ -= nWorkers;
    managerMonitor_.notifyAll();
  }

  {
    Synchronized s(workerMonitor_);
    while (activeWorkers_ != maxWorkers_)
      workerMonitor_.wait();

    {
      Synchronized s(managerMonitor_);

      FOR_EACH (it, garbage_) {
        workers_.erase(*it);
      }
      garbage_.clear();
    }
  }
}

void ThreadManager::addTask(shared_ptr<Runnable> job, int64_t /*time to live*/)
{

  {
    Synchronized s(managerMonitor_);

    if (state_ != ThreadManager::RUNNING) {
      throw ThreadException("ThreadManager: manager not running");
    }

    tasks_.push(shared_ptr<ThreadManager::Task>
                (new ThreadManager::Task(job)));

    if (idleWorkers_ > 0) {
      managerMonitor_.notify();
    }
  }
}

void ThreadManager::stop(bool /* andJoin */)
{

  bool doStop = false;

  {
    Synchronized s(managerMonitor_);

    if (state_ == ThreadManager::WAITING)
      return;

    if (state_ == RUNNING) {
      doStop = true;
      state_ = ThreadManager::STOPPING;
    }
  }

  if (doStop) {

    removeWorkers(maxWorkers_);
    state_ = ThreadManager::DONE;

  }

}

void ThreadManager::join()
{
  stop(true);
}


ThreadManager::ThreadManager(shared_ptr<ThreadFactory> threadFactory,
                             int workerCount,
                             int maxQueueSize)
  : threadFactory_(threadFactory),
    activeWorkers_(0),
    maxWorkers_(0),
    maxQueueSize_(maxQueueSize),
    idleWorkers_(0),
    state_(WAITING),
    managerMonitor_(),
    workerMonitor_()
{
  addWorkers(workerCount);
}

ThreadManager::~ThreadManager()
{
  stop(true);
}

int ThreadManager::getWorkerCount() const
{
  return maxWorkers_;
}

int ThreadManager::getActiveWorkerCount() const
{
  Synchronized s(managerMonitor_);
  return activeWorkers_;
}

int ThreadManager::getTaskCount() const
{
  Synchronized s(managerMonitor_);
  return tasks_.size();
}


} }

