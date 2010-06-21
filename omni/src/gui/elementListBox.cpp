#include "elementListBox.h"
#include "system/viewGroup/omViewGroupState.h"

ElementListBox::ElementListBox( QWidget * parent )
	: QGroupBox( "", parent )
	, mCurrentlyActiveTab(-1)
{
	mDataElementsTabs = new QTabWidget( this );

	mOverallContainer = new QVBoxLayout( this );
	mOverallContainer->addWidget( mDataElementsTabs );
}

void ElementListBox::reset()
{
	mDataElementsTabs->clear();
	setTitle("");
}

void ElementListBox::setActiveTab( QWidget * tab )
{
	mCurrentlyActiveTab = mDataElementsTabs->indexOf(tab);
}

void ElementListBox::addTab( const int preferredIndex, QWidget * tab, const QString & tabTitle)
{
	if( -1 != mDataElementsTabs->indexOf(tab) ){ 
		return; // tab was already added, don't add again
	}
	
	const int insertedIndex = mDataElementsTabs->insertTab(preferredIndex, tab, tabTitle);
	
	if( -1 == mCurrentlyActiveTab ){ 
		// first time here
		mCurrentlyActiveTab = insertedIndex;
	} else {
		// keep the tab widget fixed on the user-specified tab
		mDataElementsTabs->setCurrentIndex( mCurrentlyActiveTab );
	}
}
