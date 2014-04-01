#pragma once
#include "precomp.h"

#include "threads/taskManager.hpp"

class OmView2dManagerImpl {
 private:
  om::thread::ThreadPool threadPool_;

 public:
  OmView2dManagerImpl() { threadPool_.start(); }

  ~OmView2dManagerImpl() { threadPool_.join(); }

  template <typename T>
  inline void AddTaskBack(const T& task) {
    threadPool_.push_back(task);
  }
};
