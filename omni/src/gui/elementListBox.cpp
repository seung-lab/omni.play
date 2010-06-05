#include "elementListBox.h"

ElementListBox::ElementListBox( QWidget * parent )
	: QGroupBox( "", parent )
{
	dataElementsTabs = new QTabWidget( this );

	overallContainer = new QVBoxLayout( this );
	overallContainer->addWidget( dataElementsTabs );
}

void ElementListBox::reset()
{
	dataElementsTabs->clear();	
	setTitle("");
}

void ElementListBox::addTab( const int index, QWidget * tab, const QString & tabTitle)
{
	if( -1 != dataElementsTabs->indexOf(tab) ){
		return;
	}
	
	dataElementsTabs->insertTab(index, tab, tabTitle);	
}
