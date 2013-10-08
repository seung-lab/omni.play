#pragma once

#include <zi/concurrency/semaphore.hpp>

#include <boost/scoped_ptr.hpp>

class OmActionLogger;
class OmActionsImpl;
class OmEventManagerImpl;
class OmRandColorFile;
class OmStateManagerImpl;
class OmView2dManagerImpl;
class OmUsers;
class QWidget;

namespace om {
class users;
}

class OmProjectGlobals {
 private:
  boost::scoped_ptr<OmRandColorFile> randColorFile_;
  boost::scoped_ptr<om::users> users_;
  boost::scoped_ptr<OmView2dManagerImpl> v2dManagerImpl_;
  boost::scoped_ptr<OmStateManagerImpl> stateMan_;
  boost::scoped_ptr<OmEventManagerImpl> eventMan_;
  boost::scoped_ptr<OmActionsImpl> actions_;
  boost::scoped_ptr<OmActionLogger> actionLogger_;

  zi::semaphore fileReadThrottle_;

 public:
  OmProjectGlobals();
  ~OmProjectGlobals();

  void Init();
  void GUIinit(QWidget* widget);

  inline OmRandColorFile& RandColorFile() { return *randColorFile_; }

  inline OmView2dManagerImpl& View2dManagerImpl() { return *v2dManagerImpl_; }

  inline OmStateManagerImpl& StateManagerImpl() { return *stateMan_; }

  inline OmEventManagerImpl& EventManImpl() { return *eventMan_; }

  inline OmActionsImpl& Actions() { return *actions_; }

  inline OmActionLogger& ActionLogger() { return *actionLogger_; }

  inline zi::semaphore& FileReadSemaphore() { return fileReadThrottle_; }

  inline om::users& Users() { return *users_; }
};
