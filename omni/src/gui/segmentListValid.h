#ifndef SEGMENT_LIST_VALID_H
#define SEGMENT_LIST_VALID_H

#include "gui/segmentListBase.h"

class SegmentListValid : public SegmentListBase
{
	Q_OBJECT

 public:
	SegmentListValid( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	bool shouldSegmentBeAdded( OmSegment * );
	int getPreferredTabIndex(){ return 1; }
};

#endif
