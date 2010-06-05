#include "gui/segmentListAll.h"

SegmentListAll::SegmentListAll( QWidget * parent, 
				InspectorProperties * ip,
				ElementListBox * eb ) 
	: SegmentListBase( parent, ip, eb )
{
}

QString SegmentListAll::getTabTitle()
{
	return QString("All");
}

bool SegmentListAll::shouldSegmentBeAdded( OmSegment * seg )
{
	if( seg->GetImmutable() ) {
		return false;
	}

	return true;
}
