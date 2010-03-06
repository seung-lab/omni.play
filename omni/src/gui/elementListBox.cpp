#include "elementListBox.h"

ElementListBox::ElementListBox( QWidget * parent, QVBoxLayout * in_verticalLayout )
  : QWidget( parent )
{
	verticalLayout = in_verticalLayout;

	groupBox = NULL;
	dataElementsTabs = NULL;
	overallContainer = NULL;


	groupBox = new QGroupBox("hi");
	verticalLayout->addWidget( groupBox );

}

void ElementListBox::setTabEnabled( QWidget * tab, QString title )
{
	if( NULL != groupBox ){
		verticalLayout->removeWidget( groupBox );
		delete groupBox;
	}

	groupBox = new QGroupBox("hi");
	overallContainer = new QVBoxLayout( groupBox );
	dataElementsTabs = new QTabWidget( this );
	dataElementsTabs->addTab( tab, title );	
	overallContainer->addWidget( dataElementsTabs );

	verticalLayout->addWidget( groupBox );
}
