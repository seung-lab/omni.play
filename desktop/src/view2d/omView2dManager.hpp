#pragma once
#include "precomp.h"

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "view2d/omView2dManagerImpl.hpp"

class OmView2dManager {
 private:
  inline static OmView2dManagerImpl& impl() {
    return OmProject::Globals().View2dManagerImpl();
  }

 public:
  template <typename T>
  inline static void AddTaskBack(const T& task) {
    impl().AddTaskBack(task);
  }
};
