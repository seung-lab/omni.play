#ifndef SEGMENT_LIST_WORKING_H
#define SEGMENT_LIST_WORKING_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentLists.hpp"

class SegmentListWorking : public SegmentListBase {
Q_OBJECT

public:
	SegmentListWorking(QWidget * parent,
					   OmViewGroupState* vgs)
		: SegmentListBase(parent, vgs)
	{}

private:
	QString getTabTitle(){
		return QString("Working");
	}

	uint64_t Size() {
		return sdw_.SegmentLists()->Working().size();
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageSegments(const unsigned int offset,
					const int numToGet,
					const OmSegID startSeg)
	{
		return sdw_.SegmentLists()->Working().getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	int getPreferredTabIndex(){
		return 0;
	}

	void makeTabActiveIfContainsJumpedToSegment(){
		ElementListBox::SetActiveTab( this );
	}

	bool shouldSelectedSegmentsBeAddedToRecentList(){
		return true;
	}
};

#endif
