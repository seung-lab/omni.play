#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmSegment;
class OmSegmentCache;
class OmSegmentPage;

class OmDataArchiveSegment{
public:
	static void ArchiveRead(const OmDataPath&,
							OmSegmentPage&,
							OmSegmentCache*,
							const om::RewriteSegments rewriteSegments);

	static void ArchiveWrite(const OmDataPath&,
							 const OmSegmentPage&,
							 OmSegmentCache*);

private:
	OmDataArchiveSegment(const OmDataPath&,
						 OmSegmentPage&,
						 OmSegmentCache*);

	const OmDataPath& path_;
	OmSegmentPage& page_;
	OmSegmentCache* cache_;
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
