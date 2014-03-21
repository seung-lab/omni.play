#pragma once
#include "precomp.h"

#include "users/userSettings.h"

class OmSegmentation;

namespace om {

namespace file {
class Paths;
}

class usersImpl;

class users {
 private:
  std::unique_ptr<usersImpl> impl_;

 public:
  users(const om::file::Paths& paths);

  ~users();

  static const std::string defaultUser;

  void SwitchToDefaultUser();
  void SwitchToUser(const std::string& userName);

  QString LogFolderPath();
  void SetupFolders();

  const std::string& CurrentUser() const;
  std::string UsersFolder();
  std::string UsersRootFolder();
  userSettings& UserSettings();
};

}  // namespace om
