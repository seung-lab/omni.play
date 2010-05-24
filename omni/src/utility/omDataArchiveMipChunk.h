#ifndef OM_DATA_ARCHIVE_MIP_CHUNK_H
#define OM_DATA_ARCHIVE_MIP_CHUNK_H

#include "common/omDebug.h"
#include "system/omProjectData.h"
#include "volume/omMipChunk.h"
#include "volume/omSimpleChunk.h"

class OmDataArchiveMipChunk
{
 public:
	static void ArchiveRead( OmHdf5Path path, OmMipChunk * chunk );
	static void ArchiveWrite( OmHdf5Path path, OmMipChunk * chunk );
	static void ArchiveRead( OmHdf5Path path, OmSimpleChunk * chunk );
	static void ArchiveWrite( OmHdf5Path path, OmSimpleChunk * chunk );
};

QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
QDataStream &operator<<(QDataStream & out, const OmSimpleChunk & chunk );
QDataStream &operator>>(QDataStream & in, OmSimpleChunk & chunk );

#endif
