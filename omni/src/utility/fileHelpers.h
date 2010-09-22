#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include "common/omCommon.h"

class FileHelpers
{
 public:
	static bool isFileReadOnly(const QString& fileNameAndPath);
	static bool removeDir(const QString &dirName); // rm -rf
};

#endif
