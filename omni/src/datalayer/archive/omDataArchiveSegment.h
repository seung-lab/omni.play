#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "common/omDebug.h"

class OmDataPath;
class OmSegment;
class OmSegmentCache;

class OmDataArchiveSegment
{
 public:
	static void ArchiveRead( const OmDataPath & path, OmSegment**  page, OmSegmentCache* cache);
	static void ArchiveWrite( const OmDataPath & path, OmSegment** page, OmSegmentCache* cache);

};

#endif
