#pragma once

#include "common/common.h"

namespace om {
namespace utility {

class fileHelpers{
public:
    static bool IsFileReadOnly(const std::string& fileNameAndPath);
    static void RemoveDir(const std::string &dirName); // rm -rf
    static bool IsFolderEmpty(const std::string& dirName);
    static bool IsFolder(const std::string& dirName);

    static void MoveFile(const std::string& from, const std::string& to);
    static void RmFile(const std::string& fileNameQT);
    static void MoveAllFiles(const std::string& fromDirQT, const std::string& toDirQT);
    static void CopyFile(const std::string& from, const std::string& to);

    static bool MkDir(const std::string& dirName);

    static void Symlink(const std::string& fromDirQT, const std::string& toDirQT);
    static bool IsSymlink(const std::string& fileNameQT);
};

} // namespace utility
} // namespace om