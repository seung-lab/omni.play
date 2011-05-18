#pragma once

#include "common/omCommon.h"

class OmFileHelpers{
public:
    static bool IsFileReadOnly(const std::string& fileNameAndPath);
    static void RemoveDir(const QString &dirName); // rm -rf
    static bool IsFolderEmpty(const QString& dirName);

    static void MoveFile(const QString& from, const QString& to);
    static void MoveAllFiles(const QString& fromDirQT, const QString& toDirQT);
    static void CopyFile(const QString& from, const QString& to);

    static void MkDir(const QString& dirNameQT);

private:
    static bool removeDir(const QString &dirName); // rm -rf
};

