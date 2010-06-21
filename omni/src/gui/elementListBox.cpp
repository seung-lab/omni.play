#include "elementListBox.h"
#include "system/viewGroup/omViewGroupState.h"

ElementListBox::ElementListBox( QWidget * parent )
	: QGroupBox( "", parent )
	, currentlyActiveTab(-1)
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

int ElementListBox::addTab( const int preferredIndex, QWidget * tab, const QString & tabTitle)
{
	const int curIndex = dataElementsTabs->indexOf(tab);

	if( -1 != curIndex ){ // tab was already added
		return curIndex;
	}
	
	const int actualIndex = dataElementsTabs->insertTab(preferredIndex, tab, tabTitle);
	
	if( -1 == currentlyActiveTab ){
		currentlyActiveTab = actualIndex;
	} else {
		dataElementsTabs->setCurrentIndex( currentlyActiveTab );
	}

	return actualIndex;
}

void ElementListBox::setActiveTab( const int index )
{
	currentlyActiveTab = index;
}
