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

bool SegmentListValid::shouldSegmentBeAdded( OmSegment * seg )
{
	if( !seg->GetImmutable() ) {
		return false;
	}

	return true;
}
