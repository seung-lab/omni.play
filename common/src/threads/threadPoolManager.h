#pragma once

#include "common/stoppable.h"
#include "common/common.h"
#include "zi/utility.h"
#include <zi/mutex.hpp>

namespace om {
namespace thread {

class ThreadPoolManager : private om::SingletonBase<ThreadPoolManager> {
 private:
  zi::mutex lock_;
  std::set<common::stoppable*> pools_;

 public:
  static void StopAll();
  static void Add(common::stoppable*);
  static void Remove(common::stoppable*);

 private:
  ThreadPoolManager() {}

  ~ThreadPoolManager();

  friend class zi::singleton<ThreadPoolManager>;
};

}  // namespace threads
}  // namespace om
