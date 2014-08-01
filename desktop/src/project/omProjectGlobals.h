#pragma once
#include "precomp.h"

class OmActionLogger;
class OmActionsImpl;
namespace om {
class users;
namespace event {
class ManagerImpl;
}
namespace file {
class Paths;
}
namespace utility {
class RandColorFile;
}
}
class OmStateManagerImpl;
class OmView2dManagerImpl;
class OmUsers;
class QWidget;

class OmProjectGlobals {
 private:
  std::unique_ptr<om::utility::RandColorFile> randColorFile_;
  std::unique_ptr<om::users> users_;
  std::unique_ptr<OmView2dManagerImpl> v2dManagerImpl_;
  std::unique_ptr<OmStateManagerImpl> stateMan_;
  std::unique_ptr<OmActionsImpl> actions_;

  zi::semaphore fileReadThrottle_;

 public:
  OmProjectGlobals(const om::file::Paths& paths);
  ~OmProjectGlobals();

  void Init();
  void GUIinit(QWidget* widget);

  inline om::utility::RandColorFile& RandColorFile() { return *randColorFile_; }

  inline OmView2dManagerImpl& View2dManagerImpl() { return *v2dManagerImpl_; }

  inline OmStateManagerImpl& StateManagerImpl() { return *stateMan_; }

  inline OmActionsImpl& Actions() { return *actions_; }

  inline zi::semaphore& FileReadSemaphore() { return fileReadThrottle_; }

  inline om::users& Users() { return *users_; }
};
