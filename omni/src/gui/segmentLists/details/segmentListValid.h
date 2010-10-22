#ifndef SEGMENT_LIST_VALID_H
#define SEGMENT_LIST_VALID_H

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentLists.hpp"

class SegmentListValid : public SegmentListBase
{
	Q_OBJECT

 public:
	SegmentListValid(QWidget * parent,
					 OmViewGroupState* vgs)
		: SegmentListBase(parent, vgs)
	{}

 private:
	QString getTabTitle(){
		return QString("Valid");
	}

	uint64_t Size() {
		return currentSDW->GetSegmentLists()->Valid().size();
	}

	boost::shared_ptr<OmSegIDsListWithPage>
	getPageSegments(const unsigned int offset,
					const int numToGet,
					const OmSegID startSeg)
	{
		return currentSDW->GetSegmentLists()
			->Valid().getPageOfSegmentIDs(offset, numToGet, startSeg);
	}

	int getPreferredTabIndex(){
		return 1;
	}

	void makeTabActiveIfContainsJumpedToSegment(){
		ElementListBox::SetActiveTab( this );
	}

	bool shouldSelectedSegmentsBeAddedToRecentList(){
		return true;
	}
};

#endif
