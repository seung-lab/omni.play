#pragma once

#include "threads/omTaskManager.hpp"

class OmView2dManagerImpl {
 private:
  OmThreadPool threadPool_;

 public:
  OmView2dManagerImpl() { threadPool_.start(); }

  ~OmView2dManagerImpl() { threadPool_.join(); }

  template <typename T> inline void AddTaskBack(const T& task) {
    threadPool_.push_back(task);
  }
};
