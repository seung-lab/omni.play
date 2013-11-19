#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omSegmentationFolders.hpp"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"
#include "users/userSettings.h"

#include <QDir>

namespace om {

class usersImpl {
 private:
  const QString usersFolderRoot_;
  QString userFolder_;
  std::unique_ptr<userSettings> settings_;
  const std::string defaultUser_ = "_default";

 public:
  usersImpl() : usersFolderRoot_(usersFolderRoot()) {}

  void SwitchToDefaultUser() {
    userFolder_ = QDir(usersFolderRoot_ + QString::fromStdString(defaultUser_))
                      .absolutePath();
    loadUserSettings();
  }

  void SwitchToUser(const std::string& userName) {
    userFolder_ = QDir(usersFolderRoot_ + QString::fromStdString(userName))
                      .absolutePath();
    log_variable(userFolder_.toStdString());

    // assuming SegmentationID 1:
    if (!om::file::exists(userFolder_.toStdString() +
                          "/segmentations/segmentation1/segments")) {
      copySegmentsFromDefault();
    }
    loadUserSettings();
  }

  QString LogFolderPath() { return userFolder_ + QLatin1String("/logFiles"); }

  void SetupFolders() {
    OmFileHelpers::MkDir(userFolder_);

    fixSegmentationFolderSymlinks();
  }

  inline std::string UsersFolder() const { return userFolder_.toStdString(); }

  inline std::string UsersRootFolder() const {
    return usersFolderRoot_.toStdString();
  }

  inline userSettings& UserSettings() { return *settings_; }

 private:
  QString usersFolderRoot() { return OmFileNames::FilesFolder() + "/users/"; }

  QString makeUserSegmentsFolder(const QString& folder) {
    QString dest = folder;

    dest.replace(OmFileNames::FilesFolder(), userFolder_);

    OmFileHelpers::MkDir(dest);

    return dest += "/segments";
  }

  void fixSegmentationFolderSymlinks() {
    const auto segmentationFolders = om::fs::segmentationFolders::Find();

    FOR_EACH(iter, segmentationFolders) {
      const QString folder = iter->path;

      fixSegmentFolderSymlink(folder);
    }
  }

  void fixSegmentFolderSymlink(const QString& folder) {
    // Note this function and SetupFolders() are only called before a user is
    // selected (the user is _default), although they may look like to be
    // for any user.
    const QString oldSegmentsFolder = folder + "/segments";
    const QString userSegmentsFolder = makeUserSegmentsFolder(folder);

    if (OmFileHelpers::IsSymlink(oldSegmentsFolder)) {
      OmFileHelpers::RmFile(oldSegmentsFolder);
      OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);

    } else if (OmFileHelpers::IsFolder(oldSegmentsFolder)) {
      OmFileHelpers::MoveFile(oldSegmentsFolder, userSegmentsFolder);
      OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);

    } else {
      // no symlink present
      OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);
    }
  }

  QString oldGetVolSegmentsPathAbs(OmSegmentation* vol) {
    const QDir filesDir(vol->Folder()->GetVolPath());
    return filesDir.absolutePath() + QLatin1String("/segments/");
  }

  void loadUserSettings() {
    if (settings_.get() == NULL ||
        settings_->getFilename() != settingsFilename()) {
      std::cout << "Reloading User Settings...\n";
      settings_.reset(new userSettings(settingsFilename()));
      settings_->Load();
    }
  }

  void copySegmentsFromDefault() {
    try {
      using std::string;
      using namespace boost::filesystem;

      string userFolder = userFolder_.toStdString();
      string defaultUserFolder = usersFolderRoot_.toStdString() + defaultUser_;

      string userSegments =
          userFolder + "/segmentations/segmentation1/segments/";
      string defaultUserSegments =
          defaultUserFolder + "/segmentations/segmentation1/segments/";

      om::file::MkDir(userFolder);
      permissions(userFolder, all_all);
      om::file::MkDir(userSegments);
      permissions(userSegments, all_all);

      om::file::CopyFile(defaultUserSegments + "mstUserEdges.data",
                         userSegments + "mstUserEdges.data");
      om::file::CopyFile(defaultUserSegments + "valid_group_num.data.ver1",
                         userSegments + "valid_group_num.data.ver1");
      permissions(userSegments + "mstUserEdges.data",
                  all_all ^ owner_exe ^ group_exe ^ others_exe);
      permissions(userSegments + "valid_group_num.data.ver1",
                  all_all ^ owner_exe ^ group_exe ^ others_exe);
    }
    catch (const boost::filesystem::filesystem_error& e) {
      log_errors(project / user) << "Error creating user segementation data: "
                                 << e.what();
      throw om::IoException(e.what());
    }
  }

  inline std::string settingsFilename() {
    return userFolder_.toStdString() + "/settings.yaml";
  }
};

}  // namespace om
