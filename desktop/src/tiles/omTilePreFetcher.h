#pragma once
#include "precomp.h"

#include "threads/taskManagerTypes.h"

class OmTileDrawer;

class OmTilePreFetcher {
 public:
  OmTilePreFetcher();
  ~OmTilePreFetcher();

  void RunTasks(const std::list<OmTileDrawer*>&);
  void ClearTasks();

  void Shutdown();

 private:
  om::thread::ThreadPool mThreadPool;
};
