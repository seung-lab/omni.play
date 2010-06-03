#include "elementListBox.h"

ElementListBox::ElementListBox( QWidget * parent, QVBoxLayout * in_verticalLayout )
  : QWidget( parent )
{
	verticalLayout = in_verticalLayout;

	dataElementsTabs = NULL;

	groupBox = new QGroupBox("");
	overallContainer = new QVBoxLayout( groupBox );
	verticalLayout->addWidget( groupBox );
}

void ElementListBox::clear()
{
	if( NULL == dataElementsTabs ){
		return;
	}

	dataElementsTabs->clear();
	groupBox->setTitle( "" );
}

void ElementListBox::addTab(int index, QString boxTitle, QWidget * tab, QString tabTitle)
{
	if( NULL == dataElementsTabs ){
		setupBox();
	}

	if(-1 != dataElementsTabs->indexOf(tab)){
		return;
	}
	
	groupBox->setTitle(boxTitle);
	dataElementsTabs->insertTab(index, tab, tabTitle);	
}

void ElementListBox::setupBox()
{
	dataElementsTabs = new QTabWidget( this );
	overallContainer->addWidget( dataElementsTabs );
	
	QGroupBox * buttonBox = new QGroupBox("");
	buttonBox->setFlat(true);
	overallContainer->addWidget( buttonBox );
	QHBoxLayout * buttons = new QHBoxLayout( buttonBox );
	
}
