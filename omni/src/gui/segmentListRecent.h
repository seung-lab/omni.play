#ifndef SEGMENT_LIST_RECENT_H
#define SEGMENT_LIST_RECENT_H

#include "gui/segmentListBase.h"
#include "common/omCommon.h"

class SegmentListRecent : public SegmentListBase
{
	Q_OBJECT

 public:
	SegmentListRecent( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	OmSegIDRootType getRootSegType(){ return RECENTROOT; }
	int getPreferredTabIndex(){ return 2; }
};

#endif
