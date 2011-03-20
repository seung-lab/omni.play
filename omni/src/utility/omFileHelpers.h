#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include "common/omCommon.h"

class OmFileHelpers
{
 public:
	static bool isFileReadOnly(const std::string& fileNameAndPath);
	static void RemoveDir(const QString &dirName); // rm -rf

	static void MoveFile(const QString& from, const QString& to);

private:
	static bool removeDir(const QString &dirName); // rm -rf
};

#endif
