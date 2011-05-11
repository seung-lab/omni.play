#pragma once

#include "common/omCommon.h"

class OmFileHelpers
{
public:
    static bool isFileReadOnly(const std::string& fileNameAndPath);
    static void RemoveDir(const QString &dirName); // rm -rf
    static bool IsFolderEmpty(const QString& dirName);

    static void MoveFile(const QString& from, const QString& to);
    static void CopyFile(const QString& from, const QString& to);

private:
    static bool removeDir(const QString &dirName); // rm -rf
};

