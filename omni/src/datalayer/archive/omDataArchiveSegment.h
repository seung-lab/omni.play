#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "common/omDebug.h"

class OmDataPath;
class OmSegment;
class OmSegmentCache;

class OmDataArchiveSegment
{
 public:
	static void ArchiveRead( const OmDataPath&,
				 std::vector<OmSegment*> &,
				 boost::shared_ptr<OmSegmentCache>);
	static void ArchiveWrite( const OmDataPath&,
				  const std::vector<OmSegment*>&,
				  boost::shared_ptr<OmSegmentCache>);

};

#endif
