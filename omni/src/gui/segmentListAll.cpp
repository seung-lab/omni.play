#include "gui/segmentListAll.h"
#include "gui/elementListBox.h"

SegmentListAll::SegmentListAll( QWidget * parent, 
				InspectorProperties * ip,
				ElementListBox * eb ) 
	: SegmentListBase( parent, ip, eb )
{
}

QString SegmentListAll::getTabTitle()
{
	return QString("Working");
}

void SegmentListAll::makeTabActiveIfContainsJumpedToSegment()
{
	elementListBox->setActiveTab( this );
}
