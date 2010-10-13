#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmSegment;
class OmSegmentCache;

class OmDataArchiveSegment{
public:
	static void ArchiveRead(const OmDataPath&, std::vector<OmSegment*>&,
							boost::shared_ptr<OmSegmentCache>,
							const om::RewriteSegments rewriteSegments);

	static void ArchiveWrite(const OmDataPath&, const std::vector<OmSegment*>&,
							 boost::shared_ptr<OmSegmentCache>);

private:
	OmDataArchiveSegment(const OmDataPath&,
						 std::vector<OmSegment*>&,
						 boost::shared_ptr<OmSegmentCache>);

	const OmDataPath& path_;
	std::vector<OmSegment*>& page_;
	boost::shared_ptr<OmSegmentCache> cache_;
	const int omniFileVersion_;

	OmDataWrapperPtr dw_;
	int size_;

	void archiveRead(const om::RewriteSegments rewriteSegments);
	void archiveWrite();

	bool readSegmentsOld(const bool overrideVersion);
	void attemptOldSegmentRead();
	void readSegmentsNew();
};

#endif
