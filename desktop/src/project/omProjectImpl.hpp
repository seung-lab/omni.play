#pragma once

#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionReplayer.hpp"
#include "actions/omActions.h"
#include "common/common.h"
#include "datalayer/archive/old/omDataArchiveProject.h"
#include "datalayer/archive/project.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "project/omProjectGlobals.h"
#include "system/cache/omCacheManager.h"
#include "system/omGenericManager.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "tiles/cache/omTileCache.h"
#include "users/omGuiUserChooser.h"
#include "utility/omFileHelpers.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "datalayer/archive/project.h"
#include "system/account.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

class OmProjectImpl {
 private:
  QString projectMetadataFile_;
  QString oldHDF5projectFile_;
  OmHdf5* oldHDF5_;
  QString filesFolder_;
  QString omniFile_;

  int fileVersion_;
  bool isReadOnly_;

  OmProjectVolumes volumes_;
  std::unique_ptr<OmProjectGlobals> globals_;

 public:
  OmProjectImpl() : oldHDF5_(NULL), fileVersion_(0), isReadOnly_(false) {}

  ~OmProjectImpl() {}

  const QString& FilesFolder() { return filesFolder_; }

  const QString& OmniFile() { return omniFile_; }

  bool HasOldHDF5() const { return NULL != oldHDF5_; }

  OmHdf5* OldHDF5() {
    if (!oldHDF5_) {
      throw om::IoException("no old hdf5 file present");
    }
    return oldHDF5_;
  }

  // volume management
  OmProjectVolumes& Volumes() { return volumes_; }

  // project IO
  QString New(const QString& fileNameAndPathIn) {
    const QString fnp_rel =
        OmFileNames::AddOmniExtensionIfNeeded(fileNameAndPathIn);
    const QString fnp = QFileInfo(fnp_rel).absoluteFilePath();

    doNew(fnp);

    return fnp;
  }

  void Load(const QString& fileNameAndPath, QWidget* guiParent) {
    try {
      const QFileInfo projectFile(fileNameAndPath);
      doLoad(projectFile.absoluteFilePath(), guiParent);
    }
    catch (...) {
      globals_.reset();
      throw;
    }
  }

  void Save() {
    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()) {
      SegmentationDataWrapper(*iter).GetSegmentation().Flush();
    }

    om::data::archive::project::Write(projectMetadataFile_, this);

    globals_->Users().UserSettings().Save();

    printf("omni project saved!\n");
  }

  int GetFileVersion() const { return fileVersion_; }

  bool IsReadOnly() const { return isReadOnly_; }

  OmProjectGlobals& Globals() { return *globals_; }

 private:
  void doNew(const QString& fnp) {
    omniFile_ = fnp;
    filesFolder_ = fnp + ".files";

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
    const QString dirStr = QFileInfo(omniFile_).absolutePath();

    QDir dir(dirStr);

    if (!dir.exists()) {
      if (!dir.mkpath(dirStr)) {
        throw om::IoException("could not make path");
      }
    }
  }

  void doLoad(const QString& fnp, QWidget* guiParent) {
    if (!QFile::exists(fnp)) {
      throw om::IoException("Project file not found at");
    }

    omniFile_ = fnp;
    filesFolder_ = fnp + ".files";

    oldHDF5projectFile_ = OmFileNames::OldHDF5projectFileName();
    projectMetadataFile_ = OmFileNames::ProjectMetadataFile();

    if (!QFileInfo(omniFile_).size())
      oldHDF5projectFile_ = "";
    else
      migrateFromHdf5();

    setupGlobals();
    if (om::system::Account::IsLoggedIn()) {
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

  void doCreate() {
    QFile projectFile(omniFile_);
    if (projectFile.exists()) {
      projectFile.remove();
    }

    OmFileHelpers::RemoveDir(filesFolder_);
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
    QFile file(omniFile_);
    if (!file.open(QIODevice::WriteOnly)) {
      throw om::IoException("could not open");
    }
  }

  void migrateFromHdf5() {
    OmFileNames::MakeFilesFolder();

    OmFileHelpers::MoveFile(omniFile_, oldHDF5projectFile_);

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
