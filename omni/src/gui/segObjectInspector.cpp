#include <QtGui> 
#include "segObjectInspector.h" 
#include "system/omDebug.h"

SegObjectInspector::SegObjectInspector(QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this);
    SegmentationID = -1;
    SegmentID = -1;
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
