#pragma once
#include "precomp.h"

#include "users/userSettings.h"
#include "datalayer/paths.hpp"

class OmSegmentation;

namespace om {

namespace file {
class Paths;
}

class usersImpl;

class users {
 public:
  users(om::file::Paths paths);

  ~users();

  static const std::string defaultUser;

  void SwitchToDefaultUser();
  void SwitchToUser(const std::string& userName);

  QString LogFolderPath();
  void SetupFolders();

  const std::string& CurrentUser() const;
  om::file::Paths::Usr UserPaths();
  userSettings& UserSettings();

 private:
  std::unique_ptr<usersImpl> impl_;
};

}  // namespace om
