#pragma once

/*
 *  Manages data structures that are shared between various parts of the system.  Making centralized
 *  changes in the StateManager will send events that cause the other interested systems to be
 *  notified and synchronized.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/omCommon.h"
#include "zi/omUtility.h"

class OmChannel;
class OmSegmentation;
class OmHdf5;
class OmProjectImpl;
class OmProjectVolumes;
class OmProjectGlobals;

class OmProject : private om::singletonBase<OmProject> {
private:
    boost::scoped_ptr<OmProjectImpl> impl_;

public:
    //project IO
    static QString New(const QString& fileNameAndPath);
    static void Load(const QString& fileNameAndPath);
    static void Save();
    static void Commit();
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

    friend QDataStream &operator<<(QDataStream & out, const OmProject & p );
    friend QDataStream &operator>>(QDataStream & in, OmProject & p );

    friend class zi::singleton<OmProject>;
};

