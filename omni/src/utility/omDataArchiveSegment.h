#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "common/omDebug.h"
#include "system/omProjectData.h"

class OmDataArchiveSegment
{
 public:
	static void ArchiveRead( OmHdf5Path path, OmSegment * seg);
	static void ArchiveWrite( OmHdf5Path path, OmSegment * seg);
};

QDataStream &operator<<(QDataStream &, const OmSegment & segment );
QDataStream &operator>>(QDataStream &, OmSegment & segment );

#endif
