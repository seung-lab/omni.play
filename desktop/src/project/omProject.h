#pragma once

/*
 *  Manages data structures that are shared between various parts of the system.
 * Making centralized
 *  changes in the StateManager will send events that cause the other interested
 * systems to be
 *  notified and synchronized.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/common.h"
#include "zi/omUtility.h"
#include "datalayer/archive/project.h"

class QWidget;
class OmChannel;
class OmSegmentation;
class OmHdf5;
class OmProjectImpl;
class OmProjectVolumes;
class OmProjectGlobals;

class OmProject : private om::singletonBase<OmProject> {
 private:
  std::unique_ptr<OmProjectImpl> impl_;

 public:
  //project IO
  static QString New(const QString& fileNameAndPath);
  static void Load(const QString& fileNameAndPath, QWidget* guiParent = NULL);
  static void Save();
  static void Close();

  static bool IsReadOnly();
  static bool IsOpen();

  static const QString& FilesFolder();
  static const QString& OmniFile();

  static bool HasOldHDF5();
  static OmHdf5* OldHDF5();

  //volume management
  static OmProjectVolumes& Volumes();

  static int GetFileVersion();

  static OmProjectGlobals& Globals();

 private:
  OmProject();
  ~OmProject();

  static void setFileVersion(const int fileVersion);
  friend class OmDataArchiveProject;
  friend class om::data::archive::project;

  friend QDataStream& operator<<(QDataStream& out, const OmProject& p);
  friend QDataStream& operator>>(QDataStream& in, OmProject& p);

  friend class zi::singleton<OmProject>;
};
