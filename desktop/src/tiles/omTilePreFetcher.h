#pragma once

#include "threads/omTaskManager.hpp"

#include <list>

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
