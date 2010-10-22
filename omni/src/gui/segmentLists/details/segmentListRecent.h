#ifndef SEGMENT_LIST_RECENT_H
#define SEGMENT_LIST_RECENT_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentLists.hpp"

class SegmentListRecent : public SegmentListBase {
	Q_OBJECT

public:
	SegmentListRecent(QWidget * parent,
					  OmViewGroupState* vgs)
		: SegmentListBase(parent, vgs)
	{}

private:
	QString getTabTitle(){
		return QString("Recent");
	}

	uint64_t Size() {
		return currentSDW->GetSegmentLists()->Recent().size();
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageSegments(const unsigned int offset,
					const int numToGet,
					const OmSegID startSeg)
	{
		return currentSDW->GetSegmentLists()
			->Recent().getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	int getPreferredTabIndex(){
		return 3;
	}

	void makeTabActiveIfContainsJumpedToSegment(){
		// don't jump to this tab
	}

	bool shouldSelectedSegmentsBeAddedToRecentList(){
		return false;
	}
};

#endif
