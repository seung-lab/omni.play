#ifndef SEGMENT_LIST_UNCERTAIN_H
#define SEGMENT_LIST_UNCERTAIN_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentLists.hpp"

class SegmentListUncertain : public SegmentListBase {
	Q_OBJECT

public:
	SegmentListUncertain(QWidget * parent,
						 OmViewGroupState* vgs)
		: SegmentListBase(parent, vgs)
	{}

private:
	QString getTabTitle(){
		return QString("Uncertain");
	}

	uint64_t Size() {
		return sdw_.GetSegmentLists()->Uncertain().size();
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageSegments(const unsigned int offset,
					const int numToGet,
					const OmSegID startSeg)
	{
		return sdw_.GetSegmentLists()
			->Uncertain().getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	int getPreferredTabIndex(){
		return 2;
	}

	void makeTabActiveIfContainsJumpedToSegment(){
		// don't jump to this tab
	}

	bool shouldSelectedSegmentsBeAddedToRecentList(){
		return true;
	}
};

#endif
