#include "gui/segmentListRecent.h"

SegmentListRecent::SegmentListRecent( QWidget * parent, 
				    InspectorProperties * ip,
				    ElementListBox * eb ) 
	: SegmentListBase( parent, ip, eb )
{
}

QString SegmentListRecent::getTabTitle()
{
	return QString("Recent");
}

