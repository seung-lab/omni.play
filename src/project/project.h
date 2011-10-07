#pragma once

/*
 *  Manages data structures that are shared between various parts of the system.  Making centralized
 *  changes in the StateManager will send events that cause the other interested systems to be
 *  notified and synchronized.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/common.h"
#include "zi/omUtility.h"
#include "datalayer/archive/project.h"

class channel;
class segmentation;
class OmHdf5;
class projectImpl;
class projectVolumes;
class projectGlobals;

class project : private om::singletonBase<project> {
private:
    boost::scoped_ptr<projectImpl> impl_;

public:
    //project IO
    static std::string New(const std::string& fileNameAndPath);
    static void Load(const std::string& fileNameAndPath, QWidget* guiParent = NULL);
    static void Save();
    static void Close();

    static bool IsReadOnly();
    static bool IsOpen();

    static const std::string& FilesFolder();
    static const std::string& OmniFile();

    static bool HasOldHDF5();
    static OmHdf5* OldHDF5();

    //volume management
    static projectVolumes& Volumes();

    static int GetFileVersion();

    static projectGlobals& Globals();

private:
    project();
    ~project();

    static void setFileVersion(const int fileVersion);
    friend class dataArchiveProject;
    friend class om::data::archive::project;
    
    friend QDataStream &operator<<(QDataStream & out, const project & p );
    friend QDataStream &operator>>(QDataStream & in, project & p );

    friend class zi::singleton<project>;
};

