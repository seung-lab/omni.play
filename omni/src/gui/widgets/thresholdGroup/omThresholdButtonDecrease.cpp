#include "gui/widgets/thresholdGroup/omThresholdButtonDecrease.h"

template < class T >
OmThresholdButtonDecrease<T>::OmThresholdButtonDecrease(T * parent,
							const QString & title, 
							const QString & statusTip )
	: OmButton<T>( parent, title, statusTip, false)
{
}

template < class T >
void OmThresholdButtonDecrease<T>::doAction()
{
	OmButton<T>::mParent->decreaseThresholdByEpsilon();
	OmButton<T>::mParent->updateGui();
}
