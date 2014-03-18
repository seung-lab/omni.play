#pragma once
#include "precomp.h"

#include "threads/omTaskManager.hpp"

class OmTileDrawer;

class OmTilePreFetcher {
 public:
  OmTilePreFetcher();
  ~OmTilePreFetcher();

  void RunTasks(const std::list<OmTileDrawer*>&);
  void ClearTasks();

  void Shutdown();

 private:
  OmThreadPool mThreadPool;
};
