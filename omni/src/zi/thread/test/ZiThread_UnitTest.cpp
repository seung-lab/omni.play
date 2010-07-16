/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

#include "zi/thread/ThreadFactory.h"
#include "zi/thread/ThreadManager.h"
#include "zi/thread/Monitor.h"
#include "zi/thread/Mutex.h"

#include "zi/zunit/zunit.h"
#include "zi/base/base.h"

using boost::shared_ptr;
using namespace zi;
using namespace zi::Threads;


class MonitorTestThread : public Runnable {
public:
  MonitorTestThread(int &someInt,
                    Monitor *monitor)
    : someInt_(someInt)
  {
    monitor_ = monitor;
  }
  void run() {
    for (int i=0;i<100;i++) {
      Synchronized s(*monitor_);
      someInt_ = (i + someInt_);
    }
  }
private:
  int &someInt_;
  Monitor *monitor_;
};


ZUNIT_TEST(MonitorTest) {

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  factory->setDetached(false);
  Monitor *monitor = new Monitor();

  int total = 0;

  std::vector<shared_ptr<Thread> > threads;
  for (int i=0;i<100;i++) {
    shared_ptr<MonitorTestThread> dt(new MonitorTestThread(total, monitor));
    threads.push_back(factory->poopThread(dt));
    threads.back()->start();
  }

  FOR_EACH (it, threads)
    (*it)->join();

  {
    Synchronized s(*monitor);
    EXPECT_EQ(total, 100*50*99);
  }

  delete monitor;
}

class DumbThread : public Runnable {
public:
  DumbThread(int *incWhenDone,
             shared_ptr<RWMutex> mutex)
    : incWhenDone_(incWhenDone)
  {
    mutex_ = mutex;
  }
  void run() {
    //sleep(3);
    lastValue_ = 0;
    RWGuard g(*mutex_, true);
    for (int i=0;i<500;i++) {
      (*incWhenDone_) = (1+(*incWhenDone_));
    }
    //Sleep(1);
    for (int i=500;i<1000;i++) {
      (*incWhenDone_) = (1+(*incWhenDone_));
    }
    lastValue_ = (*incWhenDone_);
  }
  int lastValue_;
private:
  int *incWhenDone_;
  shared_ptr<RWMutex> mutex_;
};

ZUNIT_TEST(ThreadFactoryTest) {

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  factory->setDetached(false);
  shared_ptr<RWMutex> mutex(new RWMutex());

  int total = 0;

  std::vector<shared_ptr<Thread> > threads;
  std::vector<shared_ptr<DumbThread> > dthreads;
  for (int i=0;i<10;i++) {
    shared_ptr<DumbThread> dt(new DumbThread(&total, mutex));
    dthreads.push_back(dt);
    threads.push_back(factory->poopThread(dt));
    threads.back()->start();
  }

  for (int i=0;i<10;i++)
    threads[i]->join();

  for (int i=0;i<10;i++)
    EXPECT_EQ(dthreads[i]->lastValue_ % 1000, 0);

}

class MutexTestThread : public Runnable {
public:
  MutexTestThread(int &someInt,
                  shared_ptr<Mutex> mutex)
    : someInt_(someInt)
  {
    mutex_ = mutex;
  }
  void run() {
    for (int i=0;i<100;i++) {
      {
        Guard g(*mutex_);
        (someInt_) = (i + (someInt_));
      }
    }
  }
private:
  int &someInt_;
  shared_ptr<Mutex> mutex_;
};


ZUNIT_TEST(MutexTest) {

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  factory->setDetached(false);
  shared_ptr<Mutex> mutex(new Mutex());

  int total = 0;

  std::vector<shared_ptr<Thread> > threads;
  for (int i=0;i<10;i++) {
    shared_ptr<MutexTestThread> dt(new MutexTestThread(total, mutex));
    threads.push_back(factory->poopThread(dt));
    threads.back()->start();
  }

  for (int i=0;i<10;i++)
    threads[i]->join();

  {
    Guard g(*mutex);
    EXPECT_EQ(total, 10*50*99);
  }

}

ZUNIT_TEST(ThreadManagerBasicTest) {

  shared_ptr<ThreadManager> manager;

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  shared_ptr<RWMutex> mutex(new RWMutex());

  manager = shared_ptr<ThreadManager>(new ThreadManager(factory, 10));

  int total = 0;
  manager->start();
  std::vector<shared_ptr<DumbThread> > dthreads;
  std::vector<shared_ptr<Thread> > threads;
  for (int i=0;i<10000;i++) {
    shared_ptr<DumbThread> dt =
      shared_ptr<DumbThread>(new DumbThread(&total, mutex));
    dthreads.push_back(dt);
    manager->addTask(dt);
  }

  manager->join();

  EXPECT_EQ(total, 10000*1000);
  for (int i=0;i<10000;i++)
    EXPECT_EQ(dthreads[i]->lastValue_ % 1000, 0);

}

ZUNIT_TEST(ThousandThreads) {

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  factory->setDetached(false);
  shared_ptr<RWMutex> mutex(new RWMutex());

  int total = 0;

  std::vector<shared_ptr<Thread> > threads;
  for (int i=0;i<1000;i++) {
    shared_ptr<DumbThread> dt(new DumbThread(&total, mutex));
    threads.push_back(factory->poopThread(dt));
    threads.back()->start();
  }

  for (int i=0;i<1000;i++)
    threads[i]->join();

  EXPECT_EQ(total, 1000*1000);

}

int main() {
  RUN_ALL_TESTS();
}

