#ifndef SEGMENT_LIST_H
#define SEGMENT_LIST_H

#include "gui/segmentListAbstract.h"

class SegmentList : public SegmentListAbstract
{
	Q_OBJECT

public:
	SegmentList( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	bool shouldSegmentBeAdded( SegmentDataWrapper & seg );
};

#endif
