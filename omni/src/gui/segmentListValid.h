#ifndef SEGMENT_LIST_VALID_H
#define SEGMENT_LIST_VALID_H

#include "gui/segmentListBase.h"
#include "common/omCommon.h"

class SegmentListValid : public SegmentListBase
{
	Q_OBJECT

 public:
	SegmentListValid( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	OmSegIDRootType getRootSegType(){ return VALIDROOT; }
	int getPreferredTabIndex(){ return 1; }
	void makeTabActiveIfContainsJumpedToSegment();
};

#endif
