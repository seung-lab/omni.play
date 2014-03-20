#pragma once
#include "precomp.h"

#include "common/common.h"
#include "zi/utility.h"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"

class QWidget;
class OmChannel;
class OmSegmentation;
class OmHdf5;
class OmProjectImpl;
class OmProjectVolumes;
class OmProjectGlobals;
namespace om {
namespace file {
class Paths;
}
}

class OmProject : private om::SingletonBase<OmProject> {
 private:
  std::unique_ptr<OmProjectImpl> impl_;

 public:
  // project IO
  static QString New(const QString& fileNameAndPath);
  static void Load(const std::string& fileNameAndPath,
                   QWidget* guiParent = nullptr,
                   const std::string& username = std::string()) {
    return Load(QString::fromStdString(fileNameAndPath), guiParent, username);
  }
  static void Load(const QString& fileNameAndPath, QWidget* guiParent = nullptr,
                   const std::string& username = std::string());
  // SafeLoad() will check if a project is open and will close it if necessary.
  static void SafeLoad(const std::string& fileNameAndPath,
                       QWidget* guiParent = nullptr,
                       const std::string& username = std::string());
  static void Save();
  static void Close();

  static bool IsReadOnly();
  static bool IsOpen();
  static bool IsOpen(const om::file::path& fileNameAndPath,
                     const std::string& username);

  static QString FilesFolder();
  static QString OmniFile();
  static const om::file::Paths& Paths();

  static bool HasOldHDF5();
  static OmHdf5* OldHDF5();

  // volume management
  static OmProjectVolumes& Volumes();

  static OmSegmentation* GetChannel(uint id);
  static OmSegmentation* GetSegmentation(uint id);

  static int GetFileVersion();

  static OmProjectGlobals& Globals();

  static void setFileVersion(const int fileVersion);

 private:
  OmProject();
  ~OmProject();

  friend class zi::singleton<OmProject>;
};
