#pragma once

#include "common/common.h"

class OmFileHelpers{
public:
    static bool DoesFileExist(const QString& fnp);

    static bool IsFileReadOnly(const std::string& fileNameAndPath);
    static void RemoveDir(const QString &dirName); // rm -rf
    static bool IsFolderEmpty(const QString& dirName);
    static bool IsFolder(const QString& dirName);

    static void MoveFile(const QString& from, const QString& to);
    static void RmFile(const QString& fileNameQT);
    static void MoveAllFiles(const QString& fromDirQT, const QString& toDirQT);
    static void CopyFile(const QString& from, const QString& to);

    static bool MkDir(const QString& dirNameQT);
    static bool MkDir(const std::string& dirName);

    static void Symlink(const QString& fromDirQT, const QString& toDirQT);
    static bool IsSymlink(const QString& fileNameQT);

private:
    static bool removeDir(const QString &dirName); // rm -rf
};

