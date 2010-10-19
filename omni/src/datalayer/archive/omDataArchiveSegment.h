#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmSegment;
class OmSegmentPage;
class OmSegmentData;

class OmDataArchiveSegment{
public:
	static void ArchiveRead(const OmDataPath&,
							OmSegmentData*,
							const uint32_t pageSize);

private:
	OmDataArchiveSegment(const OmDataPath&,
						 OmSegmentData*,
						 const uint32_t pageSize);

	const OmDataPath& path_;
	OmSegmentData* page_;
	const uint32_t pageSize_;
	const int omniFileVersion_;
	int size_;
	OmDataWrapperPtr dw_;

	void archiveRead();

	bool readSegmentsOld(const bool overrideVersion);
	void attemptOldSegmentRead();
	void readSegmentsNew();
};

#endif
