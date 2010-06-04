#include "elementListBox.h"

ElementListBox::ElementListBox( QWidget * parent, QVBoxLayout * parentVLayout )
	: QGroupBox( "", parent )
{
	parentVLayout->addWidget(this);

	dataElementsTabs = new QTabWidget( this );

	overallContainer = new QVBoxLayout( this );
	overallContainer->addWidget( dataElementsTabs );
}

void ElementListBox::reset()
{
	dataElementsTabs->clear();
}

void ElementListBox::setTitle(const QString & boxTitle)
{
	QGroupBox::setTitle(boxTitle);
}

void ElementListBox::addTab(int index, QWidget * tab, QString tabTitle)
{
	if( -1 != dataElementsTabs->indexOf(tab) ){
		return;
	}
	
	dataElementsTabs->insertTab(index, tab, tabTitle);	
}
