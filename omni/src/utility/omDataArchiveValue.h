#ifndef OM_DATA_ARCHIVE_VALUE_H
#define OM_DATA_ARCHIVE_VALUE_H

#include "common/omDebug.h"
#include "utility/omHdf5Path.h"
#include "system/omProjectData.h"

class OmDataArchiveValue
{
 public:
	static void ArchiveRead( OmHdf5Path path, OmId * id );
	static void ArchiveWrite( OmHdf5Path path, OmId * id );
};

#endif
