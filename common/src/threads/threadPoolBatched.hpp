#pragma once

#include "threads/taskManager.hpp"

namespace om {
namespace thread {

template <class ARG, class T> struct IndivArgPolicy {

  std::function<void(T*, const ARG& arg)> func;
  T* classInstantiation;

  void run(std::shared_ptr<std::vector<ARG>> argsPtr) {
    const std::vector<ARG>& args = *argsPtr;

    const size_t size = args.size();

    for (auto i = 0; i < size; ++i) {
      func(classInstantiation, args[i]);
    }
  }
};

template <class ARG, class T> struct VectorArgPolicy {

  std::function<void(T*, const std::vector<ARG>& args)> func;
  T* classInstantiation;

  void run(std::shared_ptr<std::vector<ARG>> argsPtr) {
    const std::vector<ARG>& args = *argsPtr;

    func(classInstantiation, args);
  }
};

template <class ARG, class T, template <class, class> class ARG_RUNNER>
class ThreadPoolBatched {
 private:
  const size_t taskVecSize_;

  ARG_RUNNER<ARG, T> runner_;

  ThreadPool pool_;

  typedef std::vector<ARG> args_t;
  std::shared_ptr<args_t> args_;

  void resetArgs() {
    args_ = std::make_shared<args_t>();
    args_->reserve(taskVecSize_);
  }

 public:
  ThreadPoolBatched() : taskVecSize_(1000) {}

  ThreadPoolBatched(const int taskVecSize) : taskVecSize_(taskVecSize) {}

  ~ThreadPoolBatched() { JoinPool(); }

  template <typename U> void Start(U func, T* classInstantiation) {
    const int numWokers = utility::systemInformation::get_num_cores();
    Start(func, classInstantiation, numWokers);
  }

  template <typename U>
  void Start(U func, T* classInstantiation, const int numThreads) {
    runner_.func = func;
    runner_.classInstantiation = classInstantiation;

    pool_.start(numThreads);

    resetArgs();
  }

  inline void AddOrSpawnTasks(const ARG& t) {
    if (args_->size() >= taskVecSize_) {
      pool_.push_back(
          zi::run_fn(zi::bind(&ThreadPoolBatched::worker, this, args_)));

      resetArgs();
    }

    args_->push_back(t);
  }

  void JoinPool() {
    if (!args_->empty()) {
      pool_.push_back(
          zi::run_fn(zi::bind(&ThreadPoolBatched::worker, this, args_)));

      resetArgs();
    }

    pool_.join();
  }

 private:

  void worker(std::shared_ptr<args_t> argsPtr) { runner_.run(argsPtr); }
};

}  // namespace threads
}  // namespace om
