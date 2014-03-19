#pragma once
#include "precomp.h"

#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionReplayer.hpp"
#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/archive/old/omDataArchiveProject.h"
#include "datalayer/archive/project.h"
#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/paths.hpp"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "project/omProjectGlobals.h"
#include "system/account.h"
#include "system/cache/omCacheManager.h"
#include "system/omGenericManager.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "tiles/cache/omTileCache.h"
#include "users/omGuiUserChooser.h"
#include "utility/omFileHelpers.h"
#include "utility/segmentationDataWrapper.hpp"

class OmProjectImpl {
 private:
  QString projectMetadataFile_;
  QString oldHDF5projectFile_;
  OmHdf5* oldHDF5_;
  om::file::Paths& paths_;

  int fileVersion_;

  OmProjectVolumes volumes_;
  std::unique_ptr<OmProjectGlobals> globals_;

 public:
  OmProjectImpl(const QString& fileNameAndPathIn)
      : oldHDF5_(nullptr),
        paths_(fileNameAndPathIn.toStdString()),
        fileVersion_(0) {
    doNew();
  }

  OmProjectImpl(const QString& fileNameAndPath, QWidget* guiParent,
                const std::string& username)
      : oldHDF5_(nullptr),
        paths_(fileNameAndPathIn.toStdString()),
        fileVersion_(0) {
    doLoad(guiParent, username);
  }

  ~OmProjectImpl() {}

  QString FilesFolder() {
    if (!paths_) {
      return "";
    }
    return paths_->FilesFolder().c_str();
  }

  QString OmniFile() {
    if (!paths_) {
      return "";
    }
    return paths_->OmniFile().c_str();
  }

  const om::file::Paths& Paths() { return paths_; }

  bool HasOldHDF5() const { return nullptr != oldHDF5_; }

  OmHdf5* OldHDF5() {
    if (!oldHDF5_) {
      throw om::IoException("no old hdf5 file present");
    }
    return oldHDF5_;
  }

  // volume management
  OmProjectVolumes& Volumes() { return volumes_; }

  void Save() {
    if (IsReadOnly()) {
      log_errors << "Project saving should have been disabled in the UI.";
      return;
    }
    for (auto& seg : SegmentationDataWrapper::GetPtrVec()) {
      seg->Flush();
    }

    om::data::archive::project::Write(projectMetadataFile_, this);

    globals_->Users().UserSettings().Save();

    log_infos << "omni project saved!";
  }

  int GetFileVersion() const { return fileVersion_; }

  bool IsReadOnly() const {
    return !globals_ ||
           globals_->Users().CurrentUser() == om::users::defaultUser;
  }

  OmProjectGlobals& Globals() { return *globals_; }

 private:
  void doNew() {
    projectMetadataFile_ = OmFileNames::ProjectMetadataFile();
    oldHDF5projectFile_ = "";

    makeParentFolder();
    doCreate();
    touchEmptyProjectFile();

    setupGlobals();

    OmCacheManager::Reset();
    OmTileCache::Reset();

    OmPreferenceDefaults::SetDefaultAllPreferences();

    Save();
  }

  void makeParentFolder() {
    const QString dirStr = QFileInfo(OmniFile()).absolutePath();

    QDir dir(dirStr);

    if (!dir.exists()) {
      if (!dir.mkpath(dirStr)) {
        throw om::IoException("could not make path");
      }
    }
  }

  void doLoad(QWidget* guiParent, const std::string& username) {
    if (!om::file::Paths::IsValid(fnp.toStdString())) {
      throw om::IoException("Project file not found at" + fnp.toStdString());
    }

    oldHDF5projectFile_ = OmFileNames::OldHDF5projectFileName();
    projectMetadataFile_ = OmFileNames::ProjectMetadataFile();

    if (!QFileInfo(OmniFile()).size())
      oldHDF5projectFile_ = "";
    else
      migrateFromHdf5();

    setupGlobals();
    if (!username.empty()) {
      globals_->Users().SwitchToUser(username);
    } else if (om::system::Account::IsLoggedIn()) {
      globals_->Users().SwitchToUser(om::system::Account::username());
    } else if (guiParent) {
      OmGuiUserChooser* chooser = new OmGuiUserChooser(guiParent);
      const int userWasSelected = chooser->exec();

      if (!userWasSelected) {
        throw om::IoException("user not choosen");
      }
    }

    OmCacheManager::Reset();
    OmTileCache::Reset();
    OmActionLogger::Reset();

    if (om::file::old::exists(projectMetadataFile_.toStdString()))
      om::data::archive::project::Read(projectMetadataFile_, this);
    else
      OmDataArchiveProject::ArchiveRead(OmFileNames::ProjectMetadataFileOld(),
                                        this);

    globals_->Users().UserSettings().Load();

    OmActionReplayer::Replay();
  }

  bool IsOpen(const om::file::path& fileNameAndPath,
              const std::string& username) {
    return fileNameAndPath == OmniFile().toStdString() &&
           username == globals_->Users().CurrentUser();
  }

  void doCreate() {
    QFile projectFile(OmniFile());
    if (projectFile.exists()) {
      projectFile.remove();
    }

    OmFileHelpers::RemoveDir(FilesFolder());
    OmFileNames::MakeFilesFolder();
  }

  void openHDF5() {
    if (!QFile::exists(oldHDF5projectFile_)) {
      return;
    }

    const bool isReadOnly = true;

    oldHDF5_ = OmHdf5Manager::Get(oldHDF5projectFile_, isReadOnly);
    oldHDF5_->open();
  }

  void touchEmptyProjectFile() {
    QFile file(OmniFile());
    if (!file.open(QIODevice::WriteOnly)) {
      throw om::IoException("could not open");
    }
  }

  void migrateFromHdf5() {
    OmFileNames::MakeFilesFolder();

    OmFileHelpers::MoveFile(OmniFile(), oldHDF5projectFile_);

    touchEmptyProjectFile();

    openHDF5();

    moveProjectMetadata();

    // TODO: Save()?
  }

  void moveProjectMetadata() {
    const OmDataPath path = OmDataPaths::getProjectArchiveNameQT();
    int size;
    OmDataWrapperPtr dw = oldHDF5_->readDataset(path, &size);
    char const* const data = dw->getPtr<const char>();

    QFile newProjectMetadafile(OmFileNames::ProjectMetadataFileOld());

    if (!newProjectMetadafile.open(QIODevice::WriteOnly)) {
      throw om::IoException("could not open");
    }

    newProjectMetadafile.write(data, size);
  }

  void setFileVersion(const int v) { fileVersion_ = v; }

  void setupGlobals() {
    globals_.reset(new OmProjectGlobals());
    globals_->Init();
  }

  friend class OmProject;

  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmProjectImpl& p);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmProjectImpl& p);
  friend QDataStream& operator<<(QDataStream& out, const OmProjectImpl& p);
  friend QDataStream& operator>>(QDataStream& in, OmProjectImpl& p);
};
