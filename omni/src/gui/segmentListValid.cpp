#include "gui/segmentListValid.h"

SegmentListValid::SegmentListValid( QWidget * parent, 
				    InspectorProperties * ip,
				    ElementListBox * eb ) 
	: SegmentListBase( parent, ip, eb )
{
}

QString SegmentListValid::getTabTitle()
{
	return QString("Valid");
}

