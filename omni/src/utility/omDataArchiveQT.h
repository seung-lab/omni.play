#ifndef OM_DATA_ARCHIVE_QT_H
#define OM_DATA_ARCHIVE_QT_H

#include "common/omDebug.h"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "volume/omMipChunk.h"
#include "volume/omSimpleChunk.h"

class OmDataArchiveQT
{
 public:
	static void ArchiveRead( OmHdf5Path path, OmMipChunk * chunk );
	static void ArchiveWrite( OmHdf5Path path, OmMipChunk * chunk );

	static void ArchiveRead( OmHdf5Path path, OmSimpleChunk * chunk );
	static void ArchiveWrite( OmHdf5Path path, OmSimpleChunk * chunk );

	static void ArchiveRead( OmHdf5Path path, OmProject * project );
	static void ArchiveWrite( OmHdf5Path path, OmProject * project );

};

#endif
