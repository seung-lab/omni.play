#ifndef SEGMENT_LIST_ALL_H
#define SEGMENT_LIST_ALL_H

#include "gui/segmentListBase.h"
#include "common/omCommon.h"

class SegmentListAll : public SegmentListBase
{
	Q_OBJECT

public:
	SegmentListAll( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	OmSegIDRootType getRootSegType(){ return NOTVALIDROOT; }
	int getPreferredTabIndex(){ return 0; }
	void makeTabActiveIfContainsJumpedToSegment();
};

#endif
