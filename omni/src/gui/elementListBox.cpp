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

void ElementListBox::setTabEnabled( QString boxTitle, QWidget * tab, QString tabTitle )
{
	if( NULL == dataElementsTabs ){
		dataElementsTabs = new QTabWidget( this );
		overallContainer->addWidget( dataElementsTabs );
	}
	
	dataElementsTabs->clear();
	dataElementsTabs->addTab( tab, tabTitle );	

	groupBox->setTitle( boxTitle );
}
