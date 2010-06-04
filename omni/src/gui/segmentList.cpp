#include "gui/segmentList.h"

SegmentList::SegmentList( QWidget * parent, 
			  InspectorProperties * in_inspectorProperties,
			  ElementListBox * in_elementListBox ) 
	: SegmentListAbstract( parent, in_inspectorProperties, in_elementListBox )
{
}

QString SegmentList::getTabTitle()
{
	return QString("All");
}

bool SegmentList::shouldSegmentBeAdded( OmSegment * seg )
{
	if( seg->GetImmutable() ) {
		return false;
	}

	return true;
}
