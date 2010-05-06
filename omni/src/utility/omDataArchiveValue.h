#ifndef OM_DATA_ARCHIVE_VALUE_H
#define OM_DATA_ARCHIVE_VALUE_H

#include "common/omDebug.h"
#include "utility/omHdf5Path.h"
#include "system/omProjectData.h"

class OmDataArchiveValue
{
 public:
	static void ArchiveRead( OmHdf5Path path, QHash<SEGMENT_DATA_TYPE, OmId> & page );
	static void ArchiveWrite( OmHdf5Path path, QHash<SEGMENT_DATA_TYPE, OmId> & page );
};

#endif
