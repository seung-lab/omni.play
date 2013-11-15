#pragma once

#include <QString>
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

class OmProject : private om::SingletonBase<OmProject> {
 private:
  std::unique_ptr<OmProjectImpl> impl_;

 public:
  // project IO
  static QString New(const QString& fileNameAndPath);
  static void Load(const QString& fileNameAndPath,
                   QWidget* guiParent = nullptr);
  // SafeLoad() will check if a project is open and will close it if necessary.
  static void SafeLoad(const QString& fileNameAndPath,
                       QWidget* guiParent = nullptr);
  static void Save();
  static void Close();

  static bool IsReadOnly();
  static bool IsOpen();
  static bool IsOpen(const QString& fileNameAndPath);

  static const QString& FilesFolder();
  static const QString& OmniFile();

  static bool HasOldHDF5();
  static OmHdf5* OldHDF5();

  // volume management
  static OmProjectVolumes& Volumes();

  static int GetFileVersion();

  static OmProjectGlobals& Globals();

  static void setFileVersion(const int fileVersion);

 private:
  OmProject();
  ~OmProject();

  friend class zi::singleton<OmProject>;
};
