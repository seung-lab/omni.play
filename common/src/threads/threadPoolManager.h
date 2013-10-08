#pragma once

#include "common/stoppable.h"
#include "common/common.h"
#include "zi/utility.h"
#include "zi/mutex.h"

namespace om {
namespace threads {

class threadPoolManager : private om::singletonBase<threadPoolManager> {
 private:
  zi::mutex lock_;
  std::set<common::stoppable*> pools_;

 public:
  static void StopAll();
  static void Add(common::stoppable*);
  static void Remove(common::stoppable*);

 private:
  threadPoolManager() {}

  ~threadPoolManager();

  friend class zi::singleton<threadPoolManager>;
};

}  // namespace threads
}  // namespace om
