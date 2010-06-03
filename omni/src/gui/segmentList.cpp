#include "gui/segmentList.h"

SegmentList::SegmentList( QWidget * parent, 
			  InspectorProperties * in_inspectorProperties,
			  ElementListBox * in_elementListBox ) 
	: SegmentListAbstract( parent, in_inspectorProperties, in_elementListBox )
{
}

QString SegmentList::getTabTitle()
{
	return QString("All Segments");
}

bool SegmentList::shouldSegmentBeAdded( SegmentDataWrapper & seg )
{
	if(seg.getSegment()->GetImmutable()) {
		return false;
	}

	return true;
}
