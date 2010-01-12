#include <QtGui> 
#include "filObjectInspector.h" 

FilObjectInspector::FilObjectInspector(QWidget *parent) 
: QWidget(parent) 
{ 
    setupUi(this); 
}


void FilObjectInspector::setChannelID( const OmId channID )
{
	ChannelID = channID;
}

OmId FilObjectInspector::getChannelID()
{
	return ChannelID;
}

void FilObjectInspector::setFilterID( const OmId filterID )
{
	FilterID = filterID;
}

OmId FilObjectInspector::getFilterID()
{
	return FilterID;
}
