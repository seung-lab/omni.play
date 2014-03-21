#pragma once
#include "precomp.h"

#include "actions/io/omActionLoggerTask.hpp"
#include "actions/omActions.h"

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "threads/omTaskManager.hpp"
#include "threads/omTaskManagerTypes.h"
#include "utility/omFileHelpers.h"

#include "zi/omUtility.h"

class OmActionLogger : private om::singletonBase<OmActionLogger> {
 public:
  ~OmActionLogger() { threadPool_.join(); }

  inline static const QDir& LogFolder() { return instance().logFolder_; }

  template <typename T>
  static void save(std::shared_ptr<T> actionImpl, const std::string& str) {
    std::shared_ptr<OmActionLoggerTask<T> > task(
        new OmActionLoggerTask<T>(actionImpl, str, instance().logFolder_));

    instance().threadPool_.push_back(task);
  }

  static void Init(QString logFolderPath) {
    instance().setupLogDir(logFolderPath);
    instance().threadPool_.start(1);
  }

 private:
  QDir logFolder_;
  OmThreadPool threadPool_;

  OmActionLogger() {}

  void setupLogDir(QString logFolderPath) {
    logFolder_ = QDir(logFolderPath);

    if (logFolder_.exists()) {
      return;
    }

    OmFileHelpers::MkDir(logFolderPath);
  }

  friend class OmProjectGlobals;
  friend class zi::singleton<OmActionLogger>;
};
