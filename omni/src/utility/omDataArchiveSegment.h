#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "common/omDebug.h"
#include "system/omProjectData.h"

class OmDataArchiveSegment
{
 public:
	static void ArchiveRead( OmHdf5Path path, QHash<OmId, OmSegment*> & page );
	static void ArchiveWrite( OmHdf5Path path, QHash<OmId, OmSegment*> & page );
};

QDataStream &operator<<(QDataStream &, const QHash<OmId, OmSegment*> & page );
QDataStream &operator>>(QDataStream &, QHash<OmId, OmSegment*> & page );

#endif
