#ifndef VALID_LIST_H
#define VALID_LIST_H

#include "gui/segmentListAbstract.h"

class ValidList : public SegmentListAbstract
{
	Q_OBJECT

 public:
	ValidList( QWidget * , InspectorProperties *, ElementListBox * );

 private:
	QString getTabTitle();
	bool shouldSegmentBeAdded( OmSegment * );
};

#endif
