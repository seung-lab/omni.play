#include <QtGui> 
#include "segObjectInspector.h" 

SegObjectInspector::SegObjectInspector(QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this); 
}

void SegObjectInspector::setSegmentationID( const OmId segmenID )
{
	SegmentationID = segmenID;
}

OmId SegObjectInspector::getSegmentationID()
{
	return SegmentationID;
}

void SegObjectInspector::setSegmentID( const OmId segID )
{
	SegmentID = segID;
}

OmId SegObjectInspector::getSegmentID()
{
	return SegmentID;
}
