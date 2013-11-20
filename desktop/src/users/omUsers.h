#pragma once

#include "users/userSettings.h"

#include <QString>

class OmSegmentation;

namespace om {

class usersImpl;

class users {
 private:
  std::unique_ptr<usersImpl> impl_;

 public:
  users();

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
