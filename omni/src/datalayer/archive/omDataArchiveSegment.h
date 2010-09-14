#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmSegment;
class OmSegmentCache;
class QDataStream;

class OmDataArchiveSegment{
public:
	static void ArchiveRead(const OmDataPath&, std::vector<OmSegment*>&,
				boost::shared_ptr<OmSegmentCache>);
	static void ArchiveWrite(const OmDataPath&, const std::vector<OmSegment*>&,
				 boost::shared_ptr<OmSegmentCache>);
private:
	static bool readSegmentsOld(std::vector<OmSegment*> & page,
				    boost::shared_ptr<OmSegmentCache>,
				    OmDataWrapperPtr dw,
				    const int size,
				    const bool overrideVersion);
	static void readSegmentsNew(std::vector<OmSegment*> & page,
				    boost::shared_ptr<OmSegmentCache> cache,
				    OmDataWrapperPtr dw,
				    const int size);
};

#endif
