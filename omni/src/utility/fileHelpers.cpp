#include "fileHelpers.h"
#include <QFileInfo>

bool FileHelpers::isFileReadOnly( QString fileNameAndPath )
{
	bool isReadOnly = false;
	QFileInfo file(fileNameAndPath);
	if (file.exists() && !file.permission(QFile::WriteUser)){
		isReadOnly = true; 
	}
	
	return isReadOnly;
 }
