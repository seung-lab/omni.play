#include "fileHelpers.h"
#include <QFileInfo>
#include "common/omDebug.h"

bool FileHelpers::isFileReadOnly( QString fileNameAndPath )
{
	bool isReadOnly = false;
	QFileInfo file(fileNameAndPath);
	if (file.exists() && !file.isWritable() ){
		isReadOnly = true; 
	}
	
	debug("fileHelpers", "%s: file is read only? %d\n", __FUNCTION__, isReadOnly );
	return isReadOnly;
 }
