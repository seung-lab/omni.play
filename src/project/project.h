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

namespace om {

namespace volume {
class channel;
class segmentation;
}

namespace project {

class projectImpl;
class volumes;
class globals;

class project : private om::singletonBase<project> {
private:
    boost::scoped_ptr<projectImpl> impl_;

public:
    //project IO
    static std::string New(const std::string& fileNameAndPath);
    static void Load(const std::string& fileNameAndPath);
    static void Save();
    static void Close();

    static bool IsReadOnly();
    static bool IsOpen();

    static const std::string& FilesFolder();
    static const std::string& OmniFile();

    //volume management
    static volumes& Volumes();

    static int GetFileVersion();

    static globals& Globals();

private:
    project();
    ~project();

    static void setFileVersion(const int fileVersion);
    friend class data::archive::project;

    friend class zi::singleton<project>;
};

} // namespace project
} // namespace om
