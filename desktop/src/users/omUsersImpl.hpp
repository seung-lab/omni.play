#pragma once
#include "precomp.h"

#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"
#include "users/userSettings.h"
#include "datalayer/paths.hpp"
#include "datalayer/fs/volumeFolders.hpp"

namespace om {

class usersImpl {
 public:
  usersImpl(file::Paths paths) : paths_(paths) {}

  void SwitchToDefaultUser() {
    if (currentUser_ == om::users::defaultUser) {
      return;
    }
    currentUser_ = users::defaultUser;
    loadUserSettings();
  }

  void SwitchToUser(const std::string& userName) {
    if (currentUser_ == userName) {
      return;
    }

    currentUser_ = userName;
    const auto segmentationFolders =
        fs::VolumeFolders::FindSegmentations(paths_);
    for (auto f : segmentationFolders) {
      if (!om::file::exists(UserPaths().Segments(f.id))) {
        copySegmentsFromDefault(f.id);
      }
    }
    loadUserSettings();
  }

  const std::string& CurrentUser() const { return currentUser_; }

  void SetupFolders() { file::MkDir(UserPaths()); }

  userSettings& UserSettings() { return *settings_; }

  file::Paths::Usr UserPaths() { return paths_.UserPaths(currentUser_); }

 private:
  void loadUserSettings() {
    log_infos << "Reloading User Settings...";
    settings_.reset(new userSettings(UserPaths().Settings().string()));
    settings_->Load();
  }

  void copySegmentsFromDefault(uint8_t id) {
    using namespace boost::filesystem;
    try {
      auto up = UserPaths();
      auto defaultUp = paths_.UserPaths(users::defaultUser);

      file::MkDir(up);
      file::permissions(up, all_all);
      file::MkDir(up.Segments(id));
      permissions(up.Segments(id), all_all);
      for (auto& f : directory_iterator(defaultUp.Segments(id))) {
        auto to = up.Segments(id) / f.path().filename();
        copy_file(f, to);
        permissions(to, all_all ^ owner_exe ^ group_exe ^ others_exe);
      }
    }
    catch (const filesystem_error& e) {
      log_errors << "Error creating user segementation data: " << e.what();
      throw IoException(e.what());
    }
  }

 private:
  file::Paths paths_;
  std::string currentUser_;
  std::unique_ptr<userSettings> settings_;
};

}  // namespace om
