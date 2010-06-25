#include "gui/segmentListValid.h"
#include "gui/elementListBox.h"

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

void SegmentListValid::makeTabActiveIfContainsJumpedToSegment()
{
	elementListBox->setActiveTab( this );
}
