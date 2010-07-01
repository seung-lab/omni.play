#include "gui/widgets/thresholdGroup/omThresholdButtonIncrease.h"

template < class T >
OmThresholdButtonIncrease<T>::OmThresholdButtonIncrease(T * parent,
							const QString & title, 
							const QString & statusTip )
	: OmButton<T>( parent, title, statusTip, false)
{
}

template < class T >
void OmThresholdButtonIncrease<T>::doAction()
{
	OmButton<T>::mParent->increaseThresholdByEpsilon();
	OmButton<T>::mParent->updateGui();
}
