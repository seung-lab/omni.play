#ifndef OM_DATA_ARCHIVE_SEGMENT_H
#define OM_DATA_ARCHIVE_SEGMENT_H

#include "datalayer/omDataWrapper.h"

class OmDataPath;
class OmSegment;
class OmSegmentPage;
class OmSegmentDataV2;

class OmDataArchiveSegment{
public:
	static void ArchiveRead(const OmDataPath&,
							OmSegmentDataV2*,
							const uint32_t pageSize);

private:
	OmDataArchiveSegment(const OmDataPath&,
						 OmSegmentDataV2*,
						 const uint32_t pageSize);

	const OmDataPath& path_;
	OmSegmentDataV2* page_;
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
