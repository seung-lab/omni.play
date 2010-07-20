#include "gui/segmentListWorking.h"
#include "gui/elementListBox.h"

SegmentListWorking::SegmentListWorking( QWidget * parent, 
				InspectorProperties * ip,
				ElementListBox * eb ) 
	: SegmentListBase( parent, ip, eb )
{
}

QString SegmentListWorking::getTabTitle()
{
	return QString("Working");
}

void SegmentListWorking::makeTabActiveIfContainsJumpedToSegment()
{
	elementListBox->setActiveTab( this );
}
