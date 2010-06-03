#include "gui/validList.h"

ValidList::ValidList( QWidget * parent, 
		      InspectorProperties * in_inspectorProperties,
		      ElementListBox * in_elementListBox ) 
	: SegmentListAbstract( parent, in_inspectorProperties, in_elementListBox )
{
}

QString ValidList::getTabTitle()
{
	return QString("Valid Segments");
}

bool ValidList::shouldSegmentBeAdded( SegmentDataWrapper & seg )
{
	if(!seg.getSegment()->GetImmutable()) {
		return false;
	}

	return true;
}
