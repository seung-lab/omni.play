#ifndef SEGMENT_LIST_ALL_H
#define SEGMENT_LIST_ALL_H

#include "gui/segmentListBase.h"

class SegmentListAll : public SegmentListBase
{
	Q_OBJECT

public:
	SegmentListAll( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	bool shouldSegmentBeAdded( OmSegment * );
	int getPreferredTabIndex(){ return 0; }
};

#endif
