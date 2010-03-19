#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#include <QString>

class FileHelpers
{
 public:
	static bool isFileReadOnly( QString fileNameAndPath );
};

#endif
