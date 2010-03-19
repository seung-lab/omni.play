#include "fileHelpers.h"
#include <QFileInfo>

bool FileHelpers::isFileReadOnly( QString fileNameAndPath )
{
	bool isReadOnly = true;
	QFileInfo file(fileNameAndPath);
	if (file.permission(QFile::WriteUser)){
		isReadOnly = false; 
	}
	
	return isReadOnly;
 }
