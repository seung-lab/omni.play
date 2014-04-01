
#include "project/omProject.h"
#include "project/omProjectImpl.hpp"

OmProject::OmProject() {}

OmProject::~OmProject() {}

QString OmProject::New(const QString& fnp) {
  try {
    instance().impl_ = std::make_unique<OmProjectImpl>(fnp, "");
    instance().impl_->New();
  }
  catch (...) {
    instance().impl_.reset();
    throw;
  }
  return instance().impl_->OmniFile();
}

void OmProject::Save() { instance().impl_->Save(); }

// This will check if a project is open and will close it if necessary
void OmProject::SafeLoad(const std::string& fileNameAndPath, QWidget* guiParent,
                         const std::string& username) {
  if (IsOpen(fileNameAndPath, username)) {
    return;
  } else {
    Close();
    Load(fileNameAndPath, guiParent, username);
  }
}

void OmProject::Load(const QString& fileNameAndPath, QWidget* guiParent,
                     const std::string& username) {

  try {
    instance().impl_ =
        std::make_unique<OmProjectImpl>(fileNameAndPath, username);
    instance().impl_->Load(guiParent);
  }
  catch (...) {
    instance().impl_.reset();
    throw;
  }
}

QString OmProject::FilesFolder() { return instance().impl_->FilesFolder(); }

const om::file::Paths& OmProject::Paths() { return instance().impl_->Paths(); }

QString OmProject::OmniFile() { return instance().impl_->OmniFile(); }

bool OmProject::HasOldHDF5() { return instance().impl_->HasOldHDF5(); }

OmHdf5* OmProject::OldHDF5() { return instance().impl_->OldHDF5(); }

OmProjectVolumes& OmProject::Volumes() { return instance().impl_->Volumes(); }

int OmProject::GetFileVersion() { return instance().version_; }

void OmProject::setFileVersion(const int ver) { instance().version_ = ver; }

bool OmProject::IsReadOnly() { return instance().impl_->IsReadOnly(); }

OmProjectGlobals& OmProject::Globals() { return instance().impl_->Globals(); }

bool OmProject::IsOpen() { return static_cast<bool>(instance().impl_); }

bool OmProject::IsOpen(const om::file::path& fileNameAndPath,
                       const std::string& username) {
  if (!instance().impl_) {
    return false;
  }
  return instance().impl_->IsOpen(fileNameAndPath, username);
}

#include "actions/omActions.h"
#include "segment/omSegmentSelected.hpp"
#include "system/omOpenGLGarbageCollector.hpp"
#include "threads/omThreadPoolManager.h"

void OmProject::Close() {
  if (!IsOpen()) {
    return;
  }

  OmActions::Close();

  OmCacheManager::SignalCachesToCloseDown();
  OmThreadPoolManager::StopAll();
  zi::all_threads::join_all();

  OmTileCache::Delete();

  // OmProject must be deleted here, remaining singletons close cleanly
  instance().impl_.reset();

  // delete all singletons
  OmSegmentSelected::Delete();
  OmOpenGLGarbageCollector::Clear();
  OmPreferences::Delete();
  // OmLocalPreferences

  // close project data
  OmCacheManager::Delete();

  OmHdf5Manager::Delete();
}
