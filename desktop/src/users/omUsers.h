#pragma once

#include <boost/scoped_ptr.hpp>
#include "users/userSettings.h"

#include <QString>

class OmSegmentation;

namespace om {

class usersImpl;

class users {
 private:
  boost::scoped_ptr<usersImpl> impl_;

 public:
  users();

  ~users();

  void SwitchToDefaultUser();
  void SwitchToUser(const std::string& userName);

  QString LogFolderPath();
  void SetupFolders();

  std::string UsersFolder();
  std::string UsersRootFolder();
  userSettings& UserSettings();
};

}  // namespace om
